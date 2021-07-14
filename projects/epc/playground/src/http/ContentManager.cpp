#include <memory>

#include "ContentManager.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include "Application.h"
#include "http/HTTPServer.h"
#include "SoupOutStream.h"
#include "xml/XmlWriter.h"
#include "NetworkRequest.h"
#include "WebSocketOutStream.h"
#include "HTTPRequest.h"
#include "WebSocketRequest.h"
#include "device-settings/Settings.h"
#include "device-settings/DebugLevel.h"
#include "device-info/DeviceInformation.h"
#include <http/WebUISupport.h>
#include <clipboard/Clipboard.h>
#include <giomm.h>

ContentManager::WebsocketConnection::WebsocketConnection(SoupWebsocketConnection *c)
{
  lastSentUpdateId = 0;
  lastSelfIssuedUpdateId = 0;
  allChangesWereOracles = true;
  ws = c;
  g_object_ref(ws);
}

ContentManager::WebsocketConnection::~WebsocketConnection()
{
  DebugLevel::warning("destroying old WebsocketConnection");
  g_object_unref(ws);
}

void ContentManager::WebsocketConnection::onWebsocketRequestDone(std::shared_ptr<WebSocketRequest> request,
                                                                 tUpdateID oldID, tUpdateID newId)
{
  if(oldID == lastSelfIssuedUpdateId)
  {
    lastSelfIssuedUpdateId = newId;
  }

  if(allChangesWereOracles)
  {
    allChangesWereOracles &= request->isOracle();
  }
}

SoupWebsocketConnection *ContentManager::WebsocketConnection::getConnection()
{
  return ws;
}

UpdateDocumentContributor::tUpdateID ContentManager::WebsocketConnection::getLastSentUpdateId() const
{
  return lastSentUpdateId;
}

void ContentManager::WebsocketConnection::setLastSentUpdateId(int currentUpdateId)
{
  lastSentUpdateId = currentUpdateId;
  lastSelfIssuedUpdateId = currentUpdateId;
  allChangesWereOracles = true;
}

bool ContentManager::WebsocketConnection::canOmitOracles(int currentUpdateId) const
{
  bool allChangesTriggeredByThis = (currentUpdateId == lastSelfIssuedUpdateId);
  bool ret = allChangesTriggeredByThis && allChangesWereOracles;
  return ret;
}

ContentManager::ContentManager()
    : m_lastUpdateSentAt(std::chrono::steady_clock::now())
{
}

ContentManager::~ContentManager() = default;

void ContentManager::init()
{
  addContentSections();
}

void ContentManager::addContentSections()
{
  addContentSection(static_cast<ContentSection *>(Application::get().getWebUISupport()));
  addContentSection(static_cast<ContentSection *>(Application::get().getPresetManager()));
  addContentSection(static_cast<ContentSection *>(Application::get().getUndoScope()));
  addContentSection(static_cast<ContentSection *>(Application::get().getSettings()));
  addContentSection(static_cast<ContentSection *>(Application::get().getDeviceInformation()));
  addContentSection(static_cast<ContentSection *>(Application::get().getClipboard()));
}

void ContentManager::addContentSection(tContentSectionPtr section)
{
  m_sections.insert(section);
}

void ContentManager::handleRequest(std::shared_ptr<NetworkRequest> request)
{
  auto oldUpdateID = getUpdateIDOfLastChange();

  if(request->getPath().find("/ping/") != Glib::ustring::npos)
  {
    auto answer = request->createStream("text/plain", false);
    auto receivedPingCookie = request->getPath().substr(6);
    answer->write("/pong/");
    answer->write(receivedPingCookie);
    request->okAndComplete();
    return;
  }
  else if(request->getPath() == "/logger/log")
  {
    Glib::ustring logMsg = request->get("message");
    DebugLevel::warning("LogMsg:", logMsg);
    request->okAndComplete();
  }

  for(const tContentSectionPtr &section : m_sections)
  {
    if(tryHandlingContentSectionRequest(section, request))
    {
      auto newUpdateID = getUpdateIDOfLastChange();

      if(oldUpdateID != newUpdateID)
      {
        onUpdateIdChangedByNetworkRequest(request, oldUpdateID, newUpdateID);
      }

      return;
    }
  }

  if(auto httpRequest = std::dynamic_pointer_cast<HTTPRequest>(request))
  {
    int clientsUpdateID = std::stoi(request->get("updateID", ""));

    tUpdateID updateIDOfLastChange = getUpdateIDOfLastChange();

    if(updateIDOfLastChange == clientsUpdateID)
    {
      delayResponseUntilChanged(httpRequest);
      return;
    }
    else if(updateIDOfLastChange > clientsUpdateID)
    {
      deliverResponse(httpRequest, clientsUpdateID);
    }
    else if(updateIDOfLastChange < clientsUpdateID)
    {
      setUpdateID(clientsUpdateID);
      deliverResponse(httpRequest, clientsUpdateID);
    }
  }
}

void ContentManager::onUpdateIdChangedByNetworkRequest(std::shared_ptr<NetworkRequest> request, tUpdateID oldUpdateID,
                                                       tUpdateID newUpdateID)
{
  if(auto causer = std::dynamic_pointer_cast<WebSocketRequest>(request))
  {
    for(const auto &ws : m_webSockets)
    {
      if(causer->getSocket() == ws.second->getConnection())
      {
        ws.second->onWebsocketRequestDone(causer, oldUpdateID, newUpdateID);
      }
    }
  }
}

bool ContentManager::isSendResponsesScheduled() const
{
  return m_sendResponsesScheduled;
}

void ContentManager::connectWebSocket(const std::string &path, SoupWebsocketConnection *connection)
{
  g_signal_connect(connection, "message", G_CALLBACK(&ContentManager::onWebSocketMessage), this);
  auto ws = std::make_shared<WebsocketConnection>(connection);
  DebugLevel::warning("adding new WebsocketConnection for path", path);
  m_webSockets[path] = ws;
  feedWebSocket(ws, false);
}

void ContentManager::onWebSocketMessage(SoupWebsocketConnection *self, gint type, GBytes *message,
                                        ContentManager *pThis)
{
  try
  {
    std::shared_ptr<NetworkRequest> request(new WebSocketRequest(self, message));
    pThis->handleRequest(request);
  }
  catch(...)
  {
    DebugLevel::warning("handled exception for websocket message");
  }
}

void ContentManager::feedWebSockets(bool trustOracles)
{
  for(auto it = m_webSockets.begin(); it != m_webSockets.end();)
  {
    if(!feedWebSocket(it->second, trustOracles))
    {
      it = m_webSockets.erase(it);
      nltools::Log::warning(__PRETTY_FUNCTION__, "remove web socket connection");
    }
    else
    {
      ++it;
    }
  }
}

bool ContentManager::feedWebSocket(const tWebsocketConnection &c, bool trustOracles)
{
  auto state = soup_websocket_connection_get_state(c->getConnection());

  if(state == SOUP_WEBSOCKET_STATE_OPEN)
  {
    auto currentUpdateId = getUpdateIDOfLastChange();

    if(c->getLastSentUpdateId() != currentUpdateId)
    {
      DebugLevel::info("Updating websocket", c->getConnection(), "with document for updateID", currentUpdateId);
      WebSocketOutStream stream(c->getConnection());
      XmlWriter writer(stream);
      writeDocument(writer, c->getLastSentUpdateId(), trustOracles && c->canOmitOracles(currentUpdateId));
      c->setLastSentUpdateId(currentUpdateId);
    }
    else
    {
      DebugLevel::info("NOT updating websocket", c->getConnection(), "with document for updateID", currentUpdateId);
    }

    return true;
  }

  return false;
}

bool ContentManager::tryHandlingContentSectionRequest(tContentSectionPtr section,
                                                      std::shared_ptr<NetworkRequest> request)
{
  Glib::ustring path = request->getPath();

  if(section->contains(path))
  {
    deliverContentSectionResponse(section, request);
    return true;
  }

  return false;
}

void ContentManager::delayResponseUntilChanged(std::shared_ptr<HTTPRequest> request)
{
  addPendingMessage(request);
  request->pause();
}

void ContentManager::deliverResponse(std::shared_ptr<HTTPRequest> request,
                                     UpdateDocumentContributor::tUpdateID clientsUpdateID)
{
  request->setHeader("updateID", to_string(getUpdateIDOfLastChange()));
  auto stream = request->createStream("text/xml", false);
  XmlWriter writer(*stream);
  writeDocument(writer, clientsUpdateID);
}

void ContentManager::writeDocument(Writer &writer, UpdateDocumentContributor::tUpdateID knownRevision) const
{
  writeDocument(writer, knownRevision, false);
}

void ContentManager::writeDocument(Writer &writer, tUpdateID knownRevision, bool omitOracles) const
{
  DebugLevel::gassy(__PRETTY_FUNCTION__);

  writer.writeTag("nonlinear-world", Attribute("updateID", getUpdateIDOfLastChange()),
                  Attribute("omit-oracles", omitOracles), [&]() {
                    for(const tContentSectionPtr &section : m_sections)
                      section->writeDocument(writer, knownRevision);
                  });
}

void ContentManager::deliverContentSectionResponse(tContentSectionPtr section, std::shared_ptr<NetworkRequest> request)
{
  section->handleHTTPRequest(request, request->getPath());
}

ContentManager::tUpdateID ContentManager::onChange(uint64_t flags)
{
  ContentManager::tUpdateID id = UpdateDocumentMaster::onChange(flags);
  onSectionChanged();
  return id;
}

void ContentManager::onSectionChanged()
{
  if(!m_sendResponsesScheduled)
  {
    using namespace std::chrono;

    const auto minDelayBetweenUpdates = milliseconds(100);
    m_sendResponsesScheduled = true;

    auto now = steady_clock::now();
    auto diff = now - m_lastUpdateSentAt;

    if(diff >= minDelayBetweenUpdates || diff < steady_clock::duration::zero())
    {
      Application::get().getMainContext()->signal_idle().connect_once(
          sigc::mem_fun(this, &ContentManager::sendResponses));
    }
    else
    {
      auto millisecondsTillNextUpdate = duration_cast<milliseconds>(minDelayBetweenUpdates - diff).count();
      Application::get().getMainContext()->signal_timeout().connect_once(
          sigc::mem_fun(this, &ContentManager::sendResponses), static_cast<unsigned int>(millisecondsTillNextUpdate));
    }
  }
}

void ContentManager::sendResponses()
{
  m_sendResponsesScheduled = false;

  DebugLevel::info("sending outstanding update responses");

  auto pendingMessages = expropriateFromPendingMessages();

  for(const auto &msg : pendingMessages)
  {
    msg->unpause();
    Application::get().getHTTPServer()->handleRequest(msg);
  }

  auto dontTrustOracles = getAndResetFlags() & DontTrustOracle;
  feedWebSockets(!dontTrustOracles);

  m_lastUpdateSentAt = std::chrono::steady_clock::now();

  DebugLevel::info("sent outstanding update responses");
}

void ContentManager::onSectionMessageFinished(SoupMessage *msg)
{
  removeIfExists(msg);
}

UNDO::Scope &ContentManager::getUndoScope()
{
  return *(Application::get().getUndoScope());
}

const UNDO::Scope &ContentManager::getUndoScope() const
{
  return *(Application::get().getUndoScope());
}

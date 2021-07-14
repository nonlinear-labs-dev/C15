#include "HTTPServer.h"
#include "NetworkRequest.h"
#include "HTTPRequest.h"
#include "Application.h"
#include "xml/Writer.h"
#include "xml/XmlWriter.h"
#include "ServedFile.h"
#include "ServedJournal.h"
#include <memory>
#include "device-settings/DebugLevel.h"
#include "SoupOutStream.h"
#include <parameters/MacroControlParameter.h>
#include <nltools/threading/Throttler.h>
#include <giomm.h>
#include <fstream>
#include <nltools/system/AsyncCommandLine.h>
#include <nltools/system/SpawnAsyncCommandLine.h>
#include <nltools/system/SpawnCommandLine.h>
#include <nltools/messaging/Message.h>

HTTPServer::HTTPServer()
    : m_contentManager()
{
  startServer();
}

HTTPServer::~HTTPServer() = default;

UpdateDocumentMaster *HTTPServer::getUpdateDocumentMaster()
{
  return &m_contentManager;
}

void HTTPServer::startServer()
{
  m_server = soup_server_new(nullptr, nullptr);

  if(m_server)
    initializeServer();
  else
    g_error("Could not start the HTTP server\n");
}

void HTTPServer::initializeServer()
{
  soup_server_add_handler(m_server, nullptr, reinterpret_cast<SoupServerCallback>(serverCallback), this, nullptr);

  soup_server_add_websocket_handler(m_server, "/ws", nullptr, nullptr,
                                    reinterpret_cast<SoupServerWebsocketCallback>(&HTTPServer::webSocket), this,
                                    nullptr);
  soup_server_add_websocket_handler(m_server, "/ws-mc", nullptr, nullptr,
                                    reinterpret_cast<SoupServerWebsocketCallback>(&HTTPServer::mcWebSocket), this,
                                    nullptr);

  GError *error = nullptr;
  soup_server_listen_all(m_server, PLAYGROUND_HTTPSERVER_PORT, static_cast<SoupServerListenOptions>(0), &error);

  if(error)
  {
    nltools::Log::error("Could not start http server:", error->message);
    g_error_free(error);
  }
}

void HTTPServer::webSocket(SoupServer *, SoupWebsocketConnection *connection, const char *path, SoupClientContext *,
                           HTTPServer *pThis)
{
  pThis->m_contentManager.connectWebSocket(path, connection);
}

void HTTPServer::mcWebSocket(SoupServer *, SoupWebsocketConnection *connection, const char *, SoupClientContext *,
                             HTTPServer *pThis)
{
  pThis->m_mcviewManager.connectWebSocket(connection);
}

void HTTPServer::init()
{
  m_contentManager.init();
}

void HTTPServer::pauseMessage(SoupMessage *msg)
{
  soup_server_pause_message(m_server, msg);
}

void HTTPServer::unpauseMessage(SoupMessage *msg)
{
  soup_server_unpause_message(m_server, msg);
}

void HTTPServer::handleRequest(std::shared_ptr<NetworkRequest> request)
{
  Glib::ustring path = request->getPath();

  DebugLevel::gassy("HTTPServer::handleRequest:", path);

  if(path == "/logger/log")
  {
    Glib::ustring logMsg = request->get("message");
    DebugLevel::warning("LogMsg:", logMsg);
    request->okAndComplete();
  }
  else if(path == "/C15-journal.tar.gz")
  {
    tServedStream file(new ServedJournal(*this, std::dynamic_pointer_cast<HTTPRequest>(request)));
    m_servedStreams.push_back(file);
    file->startServing();
  }
  else if(path == "/C15-Update")
  {
    try
    {
      auto req = std::dynamic_pointer_cast<HTTPRequest>(request);
      const auto outFile = "/tmp/nonlinear-c15-update.tar";
      {
        std::ofstream out(outFile, std::ofstream::binary);
        if(out.is_open())
        {
          auto buff = req->getFlattenedBuffer();
          out.write(buff->data, buff->length);
        }
        else
        {
          nltools::Log::error("could not open /tmp to write");
        }
      }

      nltools::msg::Update::UpdateUploadedNotification msg {};
      nltools::msg::send(nltools::msg::EndPoint::BeagleBone, msg);

      request->okAndComplete();
    }
    catch(...)
    {
      request->okAndComplete();

      auto currentE = std::current_exception();
      nltools::Log::error(nltools::handle_eptr(currentE));
    }
  }
  else
  {
    if(auto http = std::dynamic_pointer_cast<HTTPRequest>(request))
    {
      if(isIndexPageAlias(path))
      {
        redirectToIndexPage(http);
        return;
      }
      else if(isStaticFileURL(path))
      {
        serveStaticFile(http);
        return;
      }
    }

    m_contentManager.handleRequest(request);
  }
}

bool HTTPServer::isIndexPageAlias(const Glib::ustring &path)
{
  return path.empty() || path == "/";
}

void HTTPServer::redirectToIndexPage(std::shared_ptr<HTTPRequest> request) const
{
  request->moved("/NonMaps/war/NonMaps.html");
}

bool HTTPServer::isStaticFileURL(const Glib::ustring &path)
{
  static const auto allowedPaths = { "/NonMaps/", "/online-help/", "/playground/resources/", "/tmp/" };
  for(auto p : allowedPaths)
    if(path.find(p) == 0)
      return true;

  return false;
}

Glib::ustring HTTPServer::getPathFromMessage(SoupMessage *msg)
{
  SoupURI *uri = soup_message_get_uri(msg);
  return soup_uri_get_path(uri);
}

void HTTPServer::serveStaticFile(std::shared_ptr<HTTPRequest> request)
{
  tServedStream file(new ServedFile(*this, request));
  m_servedStreams.push_back(file);
  file->startServing();
}

void HTTPServer::onMessageFinished(SoupMessage *msg)
{
  bool found = false;

  m_servedStreams.remove_if([&](tServedStream file) {
    if(file->matches(msg))
    {
      found = true;
      return true;
    }
    return false;
  });

  if(!found)
    m_contentManager.onSectionMessageFinished(msg);
}

void HTTPServer::serverCallback(SoupServer *server, SoupMessage *msg, const char *path, GHashTable *,
                                SoupClientContext *, HTTPServer *pThis)
{
  try
  {
    g_signal_connect(msg, "finished", reinterpret_cast<GCallback>(&HTTPServer::messageFinishedCB), pThis);
    std::shared_ptr<NetworkRequest> request(new HTTPRequest(server, msg));
    pThis->handleRequest(request);
  }
  catch(Glib::MarkupError &err)
  {
    DebugLevel::warning("handled Glib::MarkupError exception for accessing path", path, err.code());
  }
  catch(...)
  {
    DebugLevel::warning("handled exception for accessing path", path);
  }
}

void HTTPServer::messageFinishedCB(SoupMessage *msg, HTTPServer *pThis)
{
  pThis->onMessageFinished(msg);
}

const MCViewContentManager &HTTPServer::getMCViewContentManager() const
{
  return m_mcviewManager;
}

#include <Application.h>
#include "MCViewContentManager.h"
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameters/MacroControlParameter.h>
#include <tools/StringTools.h>
#include "WebSocketRequest.h"

MCViewContentManager::MCViewContentManager() = default;

void MCViewContentManager::connectWebSocket(SoupWebsocketConnection *connection)
{
  g_signal_connect(connection, "message", G_CALLBACK(&MCViewContentManager::onWebSocketMessage), this);
  m_webSockets.emplace_back(std::make_shared<WebsocketConnection>(connection));
  for(auto &param :
      Application::get().getPresetManager()->getEditBuffer()->getParameterGroupByID("MCs")->getParameters())
  {
    if(auto mc = dynamic_cast<MacroControlParameter *>(param))
    {
      using namespace std::string_literals;
      const auto idString = to_string(mc->getID());
      const auto valueD = mc->getValue().getClippedValue();
      const auto value = to_string(valueD);
      const auto uuid = "FORCE"s;
      const auto str = StringTools::buildString("MCVIEW&ID=", idString, "&VAL=", value, "&UUID=", uuid,
                                                "&NAME=", mc->getGivenName());
      soup_websocket_connection_send_text(m_webSockets.back()->getConnection(), str.c_str());
    }
  }
}

void MCViewContentManager::onWebSocketMessage(SoupWebsocketConnection *self, gint type, GBytes *message,
                                              MCViewContentManager *pThis)
{
  std::shared_ptr<WebSocketRequest> request(new WebSocketRequest(self, message));
  pThis->handleRequest(request);
}

void MCViewContentManager::handleRequest(std::shared_ptr<WebSocketRequest> request)
{
  if(request->getPath().find("set-mc") != Glib::ustring::npos)
  {
    auto id = std::stoi(request->get("id"));
    auto value = std::stod(request->get("value"));
    auto uuid = request->get("uuid");
    Application::get().getPresetManager()->getEditBuffer()->setMacroControlValueFromMCView(id, value, uuid);
  }
}

void MCViewContentManager::sendToAllWebsockets(const Glib::ustring &message) const
{
  for(auto &ws : m_webSockets)
  {
    auto state = soup_websocket_connection_get_state(ws->getConnection());

    if(state == SOUP_WEBSOCKET_STATE_OPEN)
    {
      soup_websocket_connection_send_text(ws->getConnection(), message.c_str());
    }
  }
}

MCViewContentManager::WebsocketConnection::WebsocketConnection(SoupWebsocketConnection *c)
{
  ws = c;
  g_object_ref(ws);
}

MCViewContentManager::WebsocketConnection::~WebsocketConnection()
{
  g_object_unref(ws);
}

SoupWebsocketConnection *MCViewContentManager::WebsocketConnection::getConnection()
{
  return ws;
}

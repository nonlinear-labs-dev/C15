#include <io/network/WebSocketReceiver.h>
#include <io/network/WebSocketServer.h>
#include <Application.h>
#include <cstring>

WebSocketReceiver::WebSocketReceiver(Domain d)
{
  auto cb = sigc::mem_fun(this, &WebSocketReceiver::onDataReceived);
  Application::get().getWebsocketServer()->onMessageReceived(d, cb);
}

WebSocketReceiver::~WebSocketReceiver()
{
}

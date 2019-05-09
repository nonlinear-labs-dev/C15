#include <io/network/WebSocketReceiver.h>
#include <io/network/WebSocketServer.h>
#include <Application.h>
#include <cstring>

WebSocketReceiver::WebSocketReceiver(Domain d)
{
<<<<<<< HEAD
  auto cb = sigc::mem_fun(this, &WebSocketReceiver::onDataReceived);
  Application::get().getWebsocketServer()->onMessageReceived(d, cb);
=======
  if(d == Domain::Lpc)
    Application::get().getWebsocketServer()->onMessageReceived(d, sigc::mem_fun(this, &WebSocketReceiver::onLPCMessageReceived));
  else
    Application::get().getWebsocketServer()->onMessageReceived(d, sigc::mem_fun(this, &WebSocketReceiver::onMessageReceived));
>>>>>>> layouts-reloaded
}

WebSocketReceiver::~WebSocketReceiver()
{
}
<<<<<<< HEAD
=======

void WebSocketReceiver::onMessageReceived(tMessage msg)
{
  onDataReceived(msg);
}

void parseLPCMessage(WebSocketReceiver::tMessage msg)
{
  gsize l;
  auto settingType = (uint16_t*)msg->get_data(l);

  volatile int paramEditStatus = 0;


  if(settingType[0] == 1792) {
    if(settingType[1] == 3) {
      paramEditStatus = settingType[2];
    }
  }

  if(paramEditStatus) {
    //Forgot what comes here
  }

}

void WebSocketReceiver::onLPCMessageReceived(tMessage msg)
{
  parseLPCMessage(msg);
  onDataReceived(msg);
}

>>>>>>> layouts-reloaded

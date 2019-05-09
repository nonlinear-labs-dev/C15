#pragma once

#include <io/Receiver.h>

class WebSocketReceiver : public Receiver
{
<<<<<<< HEAD
 public:
  WebSocketReceiver(Domain domain);
  virtual ~WebSocketReceiver();
=======
  public:
    WebSocketReceiver(Domain domain);
    virtual ~WebSocketReceiver();

  private:
    void onMessageReceived(tMessage msg);
    void onLPCMessageReceived(tMessage msg);
>>>>>>> layouts-reloaded
};

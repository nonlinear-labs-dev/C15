#pragma once

#include <io/Receiver.h>

class WebSocketReceiver : public Receiver
{
  public:
    WebSocketReceiver(Domain domain);
    virtual ~WebSocketReceiver();

  private:
    void onMessageReceived(tMessage msg);
    void onLPCMessageReceived(tMessage msg);
};


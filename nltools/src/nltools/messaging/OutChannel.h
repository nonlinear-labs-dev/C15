#pragma once

#include <nltools/messaging/Messaging.h>
#include <memory>

namespace nltools
{
  namespace msg
  {
    class OutChannel
    {
     public:
      OutChannel();
      virtual ~OutChannel();

      virtual void send(const SerializedMessage &msg) = 0;
      virtual bool waitForConnection(std::chrono::milliseconds timeOut) = 0;
      virtual void onConnectionEstablished(std::function<void()> cb) = 0;
      virtual bool isConnected() const = 0;
    };
  }
}

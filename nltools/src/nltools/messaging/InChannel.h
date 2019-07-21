#pragma once

#include <nltools/messaging/Messaging.h>
#include <functional>

namespace nltools
{
  namespace msg
  {
    class InChannel
    {
     public:
      using Callback = std::function<void(const SerializedMessage &)>;
      InChannel(Callback cb);
      virtual ~InChannel();

     protected:
      void onMessageReceived(const SerializedMessage &) const;

     private:
      Callback m_cb;
    };
  }
}

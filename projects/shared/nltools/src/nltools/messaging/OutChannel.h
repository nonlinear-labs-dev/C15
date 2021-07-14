#pragma once

#include <memory>
#include <chrono>
#include <functional>
#include <nltools/GlibFwd.h>

namespace nltools
{
  namespace msg
  {
    using SerializedMessage = Glib::RefPtr<Glib::Bytes>;

    class OutChannel
    {
     public:
      OutChannel();

      virtual ~OutChannel();

      virtual bool send(const SerializedMessage &msg) = 0;
      virtual void flush(const std::chrono::milliseconds timeout) = 0;
      virtual bool waitForConnection(std::chrono::milliseconds timeOut) = 0;
      virtual bool isConnected() const = 0;
    };
  }
}

#pragma once

#include <io/Bridge.h>

namespace nltools
{
  namespace msg
  {
    struct LPCMessage;
  }
}

class ToLPCBridge : public Bridge
{
  using super = Bridge;

 public:
  ToLPCBridge();

 private:
  void onMessage(const nltools::msg::LPCMessage &msg);
};

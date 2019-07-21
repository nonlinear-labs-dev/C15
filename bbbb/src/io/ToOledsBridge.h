#pragma once

#include <io/Bridge.h>

namespace nltools
{
  namespace msg
  {
    struct SetOLEDMessage;
  }
}

class ToOledsBridge : public Bridge
{
  using super = Bridge;

 public:
  ToOledsBridge();

 private:
  void onMessageReceived(const nltools::msg::SetOLEDMessage &msg);
};

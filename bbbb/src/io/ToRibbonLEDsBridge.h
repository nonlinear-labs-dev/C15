#pragma once

#include <io/Bridge.h>

namespace nltools
{
  namespace msg
  {
    struct SetRibbonLEDMessage;
  }
}

class ToRibbonLEDsBridge : public Bridge
{
  using super = Bridge;

 public:
  ToRibbonLEDsBridge();

 private:
  void onMessageReceived(const nltools::msg::SetRibbonLEDMessage &msg);
};

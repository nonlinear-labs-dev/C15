#pragma once

#include <io/Bridge.h>

namespace nltools
{
  namespace msg
  {
    struct SetPanelLEDMessage;
  }
}

class ToPanelLEDsBridge : public Bridge
{
  using super = Bridge;

 public:
  ToPanelLEDsBridge();

 private:
  void onMessageReceived(const nltools::msg::SetPanelLEDMessage &msg);
};

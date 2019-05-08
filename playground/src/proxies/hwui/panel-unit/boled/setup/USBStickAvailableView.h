#pragma once

#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>

class USBStickAvailableView : public SetupLabel
{
 public:
  USBStickAvailableView();
  virtual ~USBStickAvailableView();
  static bool usbIsReady();

 private:
  bool updateLabel();
};

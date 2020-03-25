#pragma once
#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>

class USBStickAvailableView : public SetupLabel
{
 public:
  USBStickAvailableView();
  ~USBStickAvailableView() override;

  static bool usbIsReady();
  static void setUsbAvailable(bool usbAvailable);

 private:
  static bool m_usbAvailable;
  bool updateLabel();
};

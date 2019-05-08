#pragma once

#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>

class EncoderAccelerationView : public SetupLabel
{
 private:
  typedef SetupLabel base_type;

 public:
  EncoderAccelerationView();
  virtual ~EncoderAccelerationView();

 private:
  void onSettingChanged(const Setting *s);
};

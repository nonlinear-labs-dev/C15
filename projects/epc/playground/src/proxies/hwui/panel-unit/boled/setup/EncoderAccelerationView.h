#pragma once

#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>

class Setting;

class EncoderAccelerationView : public SetupLabel
{
 private:
  typedef SetupLabel base_type;

 public:
  EncoderAccelerationView();
  ~EncoderAccelerationView() override;

 private:
  void onSettingChanged(const Setting *s);
};

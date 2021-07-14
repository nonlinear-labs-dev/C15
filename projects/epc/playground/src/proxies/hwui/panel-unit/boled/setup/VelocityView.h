#pragma once

#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>

class Setting;

class VelocityView : public SetupLabel
{
 private:
  typedef SetupLabel base_type;

 public:
  VelocityView();
  ~VelocityView() override;

 private:
  void onSettingChanged(const Setting *s);
};

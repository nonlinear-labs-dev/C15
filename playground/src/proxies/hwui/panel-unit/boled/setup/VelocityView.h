#pragma once

#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>

class VelocityView : public SetupLabel
{
 private:
  typedef SetupLabel base_type;

 public:
  VelocityView();
  virtual ~VelocityView();

 private:
  void onSettingChanged(const Setting *s);
};

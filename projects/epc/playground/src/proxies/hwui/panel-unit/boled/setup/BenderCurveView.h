#pragma once

#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>

class Setting;

class BenderCurveView : public SetupLabel
{
 private:
  typedef SetupLabel base_type;

 public:
  BenderCurveView();
  ~BenderCurveView() override;

 private:
  void onSettingChanged(const Setting *s);
};

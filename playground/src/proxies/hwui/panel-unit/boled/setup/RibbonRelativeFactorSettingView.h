#pragma once

#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>

class RibbonRelativeFactorSettingView : public SetupLabel
{
 private:
  typedef SetupLabel base_type;

 public:
  RibbonRelativeFactorSettingView();
  virtual ~RibbonRelativeFactorSettingView();

 private:
  void onSettingChanged(const Setting *s);
};

#pragma once

#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>

class Setting;

class AftertouchView : public SetupLabel
{
 private:
  typedef SetupLabel base_type;

 public:
  AftertouchView();
  ~AftertouchView() override;

 private:
  void onSettingChanged(const Setting *s);
};

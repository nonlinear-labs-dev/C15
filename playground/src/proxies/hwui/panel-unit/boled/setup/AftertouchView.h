#pragma once

#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>

class AftertouchView : public SetupLabel
{
 private:
  typedef SetupLabel base_type;

 public:
  AftertouchView();
  virtual ~AftertouchView();

 private:
  void onSettingChanged(const Setting *s);
};

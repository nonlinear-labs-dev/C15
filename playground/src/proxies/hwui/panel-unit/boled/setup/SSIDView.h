#pragma once

#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>

class SSIDView : public SetupLabel
{
 private:
  typedef SetupLabel base_type;

 public:
  SSIDView();
  virtual ~SSIDView();

 private:
  void onSettingChanged(const Setting *s);
};

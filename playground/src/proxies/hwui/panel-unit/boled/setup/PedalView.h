#pragma once

#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>

class PedalType;

class PedalView : public SetupLabel
{
 private:
  typedef SetupLabel base_type;

 public:
  PedalView(shared_ptr<PedalType> s);
  virtual ~PedalView();

 private:
  void onSettingChanged(const Setting *s);
};

#pragma once

#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>

class Setting;

class PassphraseView : public SetupLabel
{
 private:
  typedef SetupLabel base_type;

 public:
  PassphraseView();
  ~PassphraseView() override;

 private:
  void onSettingChanged(const Setting *s);
};

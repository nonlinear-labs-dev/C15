#pragma once

#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>

class PassphraseView : public SetupLabel
{
 private:
  typedef SetupLabel base_type;

 public:
  PassphraseView();
  virtual ~PassphraseView();

 private:
  void onSettingChanged(const Setting *s);
};

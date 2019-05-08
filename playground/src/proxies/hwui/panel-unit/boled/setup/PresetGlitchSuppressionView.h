#pragma once

#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>

class PresetGlitchSuppressionView : public SetupLabel
{
 private:
  typedef SetupLabel base_type;

 public:
  PresetGlitchSuppressionView();
  virtual ~PresetGlitchSuppressionView();

 private:
  void onSettingChanged(const Setting *s);
};

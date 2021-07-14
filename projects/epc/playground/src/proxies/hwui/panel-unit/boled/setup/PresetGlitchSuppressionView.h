#pragma once

#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>

class PresetGlitchSuppressionView : public SetupLabel
{
 private:
  typedef SetupLabel base_type;

 public:
  PresetGlitchSuppressionView();
  ~PresetGlitchSuppressionView() override;

 private:
  void onSettingChanged(const Setting *s);
};

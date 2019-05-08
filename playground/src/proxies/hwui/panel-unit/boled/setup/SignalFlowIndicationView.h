#pragma once

#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>
#include <device-settings/SignalFlowIndicationSetting.h>
#include <Application.h>

class SignalFlowIndicationView : public SetupLabel
{
 public:
  SignalFlowIndicationView();
  virtual ~SignalFlowIndicationView();
  void onSettingChanged(const Setting *s);
};

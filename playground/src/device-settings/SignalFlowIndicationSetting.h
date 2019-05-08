#pragma once

#include <device-settings/BooleanSetting.h>

class SignalFlowIndicationSetting : public BooleanSetting
{
 public:
  SignalFlowIndicationSetting(Settings& settings);
  virtual ~SignalFlowIndicationSetting();

  virtual bool set(tEnum m) override;

  SignalFlowIndicationSetting(const SignalFlowIndicationSetting& other) = delete;
  SignalFlowIndicationSetting& operator=(const SignalFlowIndicationSetting&) = delete;
};

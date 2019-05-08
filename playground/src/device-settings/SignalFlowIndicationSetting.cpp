#include <device-settings/SignalFlowIndicationSetting.h>

SignalFlowIndicationSetting::SignalFlowIndicationSetting(Settings& settings)
    : BooleanSetting(settings, true)
{
}

SignalFlowIndicationSetting::~SignalFlowIndicationSetting()
{
}

bool SignalFlowIndicationSetting::set(tEnum m)
{
  return BooleanSetting::set(m);
}

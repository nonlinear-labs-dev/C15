#pragma once
#include "SettingEvent.h"
#include "device-settings/DirectLoadSetting.h"

namespace DescriptiveLayouts
{
  using DirectLoadStatus = SettingEvent<bool, DirectLoadSetting>;
}
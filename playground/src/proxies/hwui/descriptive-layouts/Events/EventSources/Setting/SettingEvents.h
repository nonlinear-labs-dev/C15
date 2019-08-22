#pragma once
#include "SettingEvent.h"
#include "device-settings/AutoLoadSelectedPreset.h"

namespace DescriptiveLayouts {
  using DirectLoadStatus = SettingEvent<bool, AutoLoadSelectedPreset>;

}
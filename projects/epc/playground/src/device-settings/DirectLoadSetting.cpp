#include "DirectLoadSetting.h"
#include <device-settings/Settings.h>

DirectLoadSetting::DirectLoadSetting(Settings &settings)
    : BooleanSetting(settings, false)
{
}

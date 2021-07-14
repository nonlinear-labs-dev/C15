#include "AutoStartRecorderSetting.h"
#include <device-settings/Settings.h>

AutoStartRecorderSetting::AutoStartRecorderSetting(Settings& s)
    : BooleanSetting(s, true)
{
}

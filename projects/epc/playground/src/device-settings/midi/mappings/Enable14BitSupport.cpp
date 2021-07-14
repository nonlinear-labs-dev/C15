#include "Enable14BitSupport.h"
#include <device-settings/Settings.h>

Enable14BitSupport::Enable14BitSupport(Settings& s)
    : BooleanSetting(s, true)
{
}

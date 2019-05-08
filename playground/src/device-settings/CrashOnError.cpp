#include "CrashOnError.h"
#include "Settings.h"

CrashOnError::CrashOnError(Settings &parent)
    : BooleanSetting(parent, false)
{
}
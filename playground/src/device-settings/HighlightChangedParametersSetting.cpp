#include <Application.h>
#include "HighlightChangedParametersSetting.h"

HighlightChangedParametersSetting::HighlightChangedParametersSetting(Settings& parent)
    : BooleanSetting(parent, true)
{
}

ForceHighlightChangedParametersSetting::ForceHighlightChangedParametersSetting(Settings& parent)
    : BooleanSetting(parent, false)
{
}

bool ForceHighlightChangedParametersSetting::persistent() const
{
  return false;
}
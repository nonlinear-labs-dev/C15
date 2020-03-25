#include "PitchbendParameter.h"
#include <proxies/hwui/panel-unit/boled/parameter-screens/PlayControlParameterLayouts.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterInfoLayout.h>
#include <glib.h>

ReturnMode PitchbendParameter::getReturnMode() const
{
  return ReturnMode::Center;
}

Layout *PitchbendParameter::createLayout(FocusAndMode focusAndMode) const
{
  switch(focusAndMode.mode)
  {
    case UIMode::Info:
      return new ParameterInfoLayout();

    case UIMode::Edit:
      return new PitchbendParameterEditLayout2();

    case UIMode::Select:
    default:
      return new PitchbendParameterSelectLayout2();
  }

  g_return_val_if_reached(nullptr);
}

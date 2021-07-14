#include "UnmodulateableMonoParameter.h"
#include <proxies/hwui/panel-unit/boled/parameter-screens/MonoModeParameterLayouts.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/UnmodulatebaleParameterLayouts.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterInfoLayout.h>

UnmodulateableMonoParameter::UnmodulateableMonoParameter(ParameterGroup *group, const ParameterId &id,
                                                         const ScaleConverter *scaling, tControlPositionValue def,
                                                         tControlPositionValue coarseDenominator,
                                                         tControlPositionValue fineDenominator)
    : Parameter { group, id, scaling, def, coarseDenominator, fineDenominator }
{
}

Layout *UnmodulateableMonoParameter::createLayout(FocusAndMode focusAndMode) const
{
  switch(focusAndMode.mode)
  {
    case UIMode::Select:
    default:
      return new MonoModeParameterLayout();
    case UIMode::Edit:
      return new UnmodulateableParameterEditLayout2();
    case UIMode::Info:
      return new ParameterInfoLayout();
  }
}

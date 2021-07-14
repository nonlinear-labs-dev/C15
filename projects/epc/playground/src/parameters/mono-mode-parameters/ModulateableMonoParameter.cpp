#include <proxies/hwui/panel-unit/boled/parameter-screens/MonoModeParameterLayouts.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ModulateableParameterLayouts.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterInfoLayout.h>
#include "ModulateableMonoParameter.h"

ModulateableMonoParameter::ModulateableMonoParameter(ParameterGroup *group, const ParameterId &id,
                                                     const ScaleConverter *scaling, tControlPositionValue def,
                                                     tControlPositionValue coarseDenominator,
                                                     tControlPositionValue fineDenominator)
    : ModulateableParameter { group, id, scaling, def, coarseDenominator, fineDenominator }
{
}

Layout *ModulateableMonoParameter::createLayout(FocusAndMode focusAndMode) const
{
  switch(focusAndMode.mode)
  {
    case UIMode::Select:
    default:
      return new MonoModeModulateableParameterLayout();
    case UIMode::Edit:
      return new ModulateableParameterEditLayout2();
    case UIMode::Info:
      return new ParameterInfoLayout();
  }
}

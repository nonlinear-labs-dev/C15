#include "MacroControlSmoothingParameter.h"
#include <proxies/hwui/panel-unit/boled/parameter-screens/MacroControlSmoothingParameterLayout.h>
#include <groups/MacroControlsGroup.h>

Layout* MacroControlSmoothingParameter::createLayout(FocusAndMode focusAndMode) const
{
  if(focusAndMode.mode == UIMode::Select)
  {
    return new MacroControlSmoothingParameterLayout();
  }
  return Parameter::createLayout(focusAndMode);
}

ParameterId MacroControlSmoothingParameter::getMC() const
{
  return MacroControlsGroup::smoothingIdToMCId(getID());
}
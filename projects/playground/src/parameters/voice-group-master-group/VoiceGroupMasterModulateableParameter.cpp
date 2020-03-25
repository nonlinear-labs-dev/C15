#include <proxies/hwui/panel-unit/boled/parameter-screens/DualVoiceGroupMasterAndSplitPointLayout.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterInfoLayout.h>
#include <utility>
#include "VoiceGroupMasterModulateableParameter.h"
#include <groups/ParameterGroup.h>

VoiceGroupMasterModulateableParameter::VoiceGroupMasterModulateableParameter(
    ParameterGroup *group, const ParameterId &id, const ScaleConverter *scaling, tControlPositionValue def,
    tControlPositionValue coarseDenominator, tControlPositionValue fineDenominator)
    : ModulateableParameter(group, id, scaling, def, coarseDenominator, fineDenominator)
{
}

Layout *VoiceGroupMasterModulateableParameter::createLayout(FocusAndMode focusAndMode) const
{
  switch(focusAndMode.mode)
  {
    case UIMode::Info:
      return new ParameterInfoLayout();
    case UIMode::Edit:
      return new ModulateableParameterEditLayout2();
    case UIMode::Select:
    default:
      return new DualVoiceGroupMasterAndSplitPointLayout();
  }
}

Layout *VoiceGroupMasterUnmodulateableParameter::createLayout(FocusAndMode focusAndMode) const
{
  switch(focusAndMode.mode)
  {
    case UIMode::Info:
      return new ParameterInfoLayout();
    case UIMode::Edit:
      return new UnmodulateableParameterEditLayout2();
    case UIMode::Select:
    default:
      return new DualVoiceGroupMasterAndSplitPointLayout();
  }
}

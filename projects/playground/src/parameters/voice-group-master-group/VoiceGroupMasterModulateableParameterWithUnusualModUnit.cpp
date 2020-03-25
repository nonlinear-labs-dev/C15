#include <proxies/hwui/panel-unit/boled/parameter-screens/ModulateableParameterLayouts.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/DualVoiceGroupMasterAndSplitPointLayout.h>
#include "VoiceGroupMasterModulateableParameterWithUnusualModUnit.h"
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterInfoLayout.h>
#include <groups/ParameterGroup.h>

VoiceGroupMasterModulateableParameterWithUnusualModUnit::VoiceGroupMasterModulateableParameterWithUnusualModUnit(
    ParameterGroup* group, const ParameterId& id, const ScaleConverter* scale, const ScaleConverter* modScale,
    tControlPositionValue def, int coarse, int fine)
    : ModulateableParameterWithUnusualModUnit(group, id, scale, modScale, def, coarse, fine)
{
}

Layout* VoiceGroupMasterModulateableParameterWithUnusualModUnit::createLayout(FocusAndMode focusAndMode) const
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

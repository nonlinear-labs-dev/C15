#include <proxies/hwui/panel-unit/boled/parameter-screens/ModulateableParameterLayouts.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/DualVoiceGroupMasterAndSplitPointLayout.h>
#include "VoiceGroupMasterParameterWithUnusualModUnit.h"
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterInfoLayout.h>

VoiceGroupMasterParameterWithUnusualModUnit::VoiceGroupMasterParameterWithUnusualModUnit(
    ParameterGroup* group, const ParameterId& id, const ScaleConverter* scale, const ScaleConverter* modScale,
    tControlPositionValue def, int coarse, int fine, Glib::ustring longName, Glib::ustring shortName)
    : ModulateableParameterWithUnusualModUnit(group, id, scale, modScale, def, coarse, fine)
    , m_shortname{ shortName }
    , m_longname{ longName }
{
}

Glib::ustring VoiceGroupMasterParameterWithUnusualModUnit::getLongName() const
{
  return m_longname;
}

Glib::ustring VoiceGroupMasterParameterWithUnusualModUnit::getShortName() const
{
  return m_shortname;
}

Glib::ustring VoiceGroupMasterParameterWithUnusualModUnit::getGroupAndParameterName() const
{
  return getParentGroup()->getShortName() + getLongName();
}

DFBLayout* VoiceGroupMasterParameterWithUnusualModUnit::createLayout(FocusAndMode focusAndMode) const
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

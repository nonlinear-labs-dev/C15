#include "SplitPointParameter.h"
#include <parameters/scale-converters/SplitPointScaleConverter.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ModulateableDualVoiceGroupMasterAndSplitPointLayout.h>
#include "groups/ParameterGroup.h"
#include "proxies/hwui/HWUI.h"
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterInfoLayout.h>
#include <parameters/scale-converters/dimension/SplitPointDimension.h>
#include <Application.h>

SplitPointParameter::SplitPointParameter(ParameterGroup *group, const ParameterId &id)
    : ModulateableParameterWithUnusualModUnit(group, id, ScaleConverter::get<SplitPointScaleConverter>(),
                                              ScaleConverter::get<SplitPointScaleConverter>(), 0.5, 59, 59)
{
}

Layout *SplitPointParameter::createLayout(FocusAndMode focusAndMode) const
{
  switch(focusAndMode.mode)
  {
    case UIMode::Info:
      return new ParameterInfoLayout();
    case UIMode::Edit:
      return new ModulateableParameterEditLayout2();
    case UIMode::Select:
    default:
      return new ModulateableDualVoiceGroupMasterAndSplitPointLayout();
  }
}

Glib::ustring SplitPointParameter::getDisplayValue(VoiceGroup vg) const
{
  return SplitPointDimension::stringizeSplitPointDisplay(getValue().getRawValue(), vg);
}

Glib::ustring SplitPointParameter::getDisplayString() const
{
  return getDisplayValue(Application::get().getHWUI()->getCurrentVoiceGroup());
}

Glib::ustring SplitPointParameter::stringizeModulationAmount(tControlPositionValue amount) const
{
  return std::to_string(static_cast<int>(60 * amount)) + " st";
}

Glib::ustring SplitPointParameter::modulationValueToDisplayString(tControlPositionValue v) const
{
  auto vg = Application::get().getHWUI()->getCurrentVoiceGroup();
  return SplitPointDimension::get().stringizeSplitPointDisplay(v, vg);
}

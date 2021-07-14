#include <Application.h>
#include <glibmm/ustring.h>
#include <parameters/ModulateableParameter.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/LowerModulationBoundLabel.h>
#include <utility>

class MacroControlParameter;

LowerModulationBoundLabel::LowerModulationBoundLabel(const Rect &r)
    : super(r)
{
}

LowerModulationBoundLabel::~LowerModulationBoundLabel() = default;

void LowerModulationBoundLabel::updateText(MacroControlParameter *mcParam, ModulateableParameter *modulatedParam)
{
  auto rangeDisplayValues = modulatedParam->getModRangeAsDisplayValues();
  const auto fine = isHighlight() && Application::get().getHWUI()->isModifierSet(ButtonModifier::FINE);

  if(fine)
    setText(StringAndSuffix { rangeDisplayValues.first, " F" });
  else
    setText(StringAndSuffix { rangeDisplayValues.first });
}

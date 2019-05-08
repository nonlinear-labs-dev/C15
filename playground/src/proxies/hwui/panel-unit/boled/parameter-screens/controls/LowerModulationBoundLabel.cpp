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

LowerModulationBoundLabel::~LowerModulationBoundLabel()
{
}

void LowerModulationBoundLabel::updateText(MacroControlParameter *mcParam, ModulateableParameter *modulatedParam)
{
  auto rangeDisplayValues = modulatedParam->getModRangeAsDisplayValues();

  if(isHighlight() && Application::get().getHWUI()->isModifierSet(ButtonModifier::FINE))
    setText(rangeDisplayValues.first + " F", 2);
  else
    setText(rangeDisplayValues.first, 0);
}

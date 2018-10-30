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
  std::string valueWithoutSpaces = rangeDisplayValues.first;
  valueWithoutSpaces.erase(remove_if(valueWithoutSpaces.begin(), valueWithoutSpaces.end(), [](auto c){ return std::isspace(c);}), valueWithoutSpaces.end());

  if(isHighlight() && Application::get().getHWUI()->isModifierSet(ButtonModifier::FINE))
    setText(valueWithoutSpaces + "F", 1);
  else
    setText(valueWithoutSpaces, 0);
}

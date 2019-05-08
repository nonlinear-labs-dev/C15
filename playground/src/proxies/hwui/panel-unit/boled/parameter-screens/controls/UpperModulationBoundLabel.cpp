#include <Application.h>
#include <glibmm/ustring.h>
#include <parameters/ModulateableParameter.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/UpperModulationBoundLabel.h>
#include <utility>

class MacroControlParameter;

UpperModulationBoundLabel::UpperModulationBoundLabel(const Rect &r)
    : super(r)
{
}

UpperModulationBoundLabel::~UpperModulationBoundLabel()
{
}

void UpperModulationBoundLabel::updateText(MacroControlParameter *mcParam, ModulateableParameter *modulatedParam)
{
  auto rangeDisplayValues = modulatedParam->getModRangeAsDisplayValues();

  if(isHighlight() && Application::get().getHWUI()->isModifierSet(ButtonModifier::FINE))
    setText(rangeDisplayValues.second + " F", 2);
  else
    setText(rangeDisplayValues.second, 0);
}

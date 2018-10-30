#include <Application.h>
#include <glibmm/ustring.h>
#include <parameters/ModulateableParameter.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/UpperModulationBoundLabel.h>
#include <utility>

class MacroControlParameter;

UpperModulationBoundLabel::UpperModulationBoundLabel (const Rect &r) :
    super (r)
{
}

UpperModulationBoundLabel::~UpperModulationBoundLabel ()
{
}

void UpperModulationBoundLabel::updateText (MacroControlParameter *mcParam, ModulateableParameter *modulatedParam)
{
  auto rangeDisplayValues = modulatedParam->getModRangeAsDisplayValues();
  std::string valueWithoutSpaces = rangeDisplayValues.second;
  valueWithoutSpaces.erase(remove_if(valueWithoutSpaces.begin(), valueWithoutSpaces.end(), [](auto c){ return std::isspace(c);}), valueWithoutSpaces.end());

  if (isHighlight () && Application::get ().getHWUI ()->isModifierSet(ButtonModifier::FINE))
    setText (valueWithoutSpaces + "F", 1);
  else
    setText (valueWithoutSpaces, 0);
}


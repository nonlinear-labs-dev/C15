#include <Application.h>
#include <glibmm/ustring.h>
#include <parameters/ModulateableParameter.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/UpperModulationBoundLabel.h>
#include <utility>
#include <tools/StringTools.h>

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
  std::string valueWithoutSpaces = StringTools::removeSpaces(rangeDisplayValues.second);

  if (isHighlight () && Application::get ().getHWUI ()->isModifierSet(ButtonModifier::FINE))
    setText (valueWithoutSpaces + "F", 1);
  else
    setText (valueWithoutSpaces, 0);
}


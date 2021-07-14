#include <Application.h>
#include <glibmm/ustring.h>
#include <libundo/undo/Scope.h>
#include <libundo/undo/Transaction.h>
#include <libundo/undo/TransactionCreationScope.h>
#include <parameters/ModulateableParameter.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/CurrentModulatedValueLabel.h>

class MacroControlParameter;

CurrentModulatedValueLabel::CurrentModulatedValueLabel(const Rect &r)
    : super(r)
{
}

void CurrentModulatedValueLabel::updateText(MacroControlParameter *mcParam, ModulateableParameter *modulatedParam)
{
  if(isHighlight() && Application::get().getHWUI()->isModifierSet(ButtonModifier::FINE))
    setText(StringAndSuffix { modulatedParam->getDisplayString(), " F" });
  else
    setText(StringAndSuffix { modulatedParam->getDisplayString() });
}

bool CurrentModulatedValueLabel::onRotary(int inc, ButtonModifiers modifiers)
{
  if(auto p = getModulatedParameter())
  {
    ParameterUseCases useCase(p);
    useCase.incDec(inc, modifiers[ButtonModifier::FINE], modifiers[ButtonModifier::SHIFT]);
    return true;
  }
  return false;
}

void CurrentModulatedValueLabel::setDefault()
{
  if(auto param = getModulatedParameter())
  {
    ParameterUseCases useCase(param);
    useCase.setDefault();
  }
}

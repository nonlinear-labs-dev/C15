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

CurrentModulatedValueLabel::~CurrentModulatedValueLabel()
{
}

void CurrentModulatedValueLabel::updateText(MacroControlParameter *mcParam, ModulateableParameter *modulatedParam)
{
  if(isHighlight() && Application::get().getHWUI()->isModifierSet(ButtonModifier::FINE))
    setText(modulatedParam->getDisplayString() + " F", 2);
  else
    setText(modulatedParam->getDisplayString(), 0);
}

bool CurrentModulatedValueLabel::onRotary(int inc, ButtonModifiers modifiers)
{
  if(auto p = getModulatedParameter())
  {
    auto scope = p->getUndoScope().startContinuousTransaction(p, "Set '%0'", p->getGroupAndParameterName());
    p->stepCPFromHwui(scope->getTransaction(), inc, modifiers);
    return true;
  }
  return false;
}

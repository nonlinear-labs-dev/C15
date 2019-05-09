#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulationBoundControl.h>
#include <Application.h>
#include <proxies/hwui/buttons.h>
#include <parameters/ModulateableParameter.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>

bool ModulationBoundControl::onButton(Buttons i, bool down, ButtonModifiers)
{
  switch(i)
  {
    case Buttons::BUTTON_DEFAULT:
      if(down)
      {
        if(auto modulatedParam = dynamic_cast<ModulateableParameter *>(
               Application::get().getPresetManager()->getEditBuffer()->getSelected()))
        {
          auto &undoScope = modulatedParam->getUndoScope();
          auto scope = undoScope.startContinuousTransaction(modulatedParam, "Set MC Amount for '%0'",
                                                            modulatedParam->getGroupAndParameterName());
          auto transaction = scope->getTransaction();
          modulatedParam->setModulationAmount(transaction, 0);
        }
        return true;
      }
      break;
  }
  return false;
}

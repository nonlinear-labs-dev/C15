#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulationBoundControl.h>
#include <Application.h>
#include <proxies/hwui/buttons.h>
#include <parameters/ModulateableParameter.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <libundo/undo/Scope.h>
#include <proxies/hwui/HWUI.h>

bool ModulationBoundControl::onButton(Buttons i, bool down, ButtonModifiers)
{
  switch(i)
  {
    case Buttons::BUTTON_DEFAULT:
      if(down)
      {
        auto eb = Application::get().getPresetManager()->getEditBuffer();
        if(auto modulatedParam
           = dynamic_cast<ModulateableParameter *>(eb->getSelected(getHWUI()->getCurrentVoiceGroup())))
        {
          ModParameterUseCases useCase(modulatedParam);
          useCase.setModulationAmount(0);
        }
        return true;
      }
      break;
  }
  return false;
}

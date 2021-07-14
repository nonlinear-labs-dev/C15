#include "MacroControlSmoothingParameterLayout.h"
#include "proxies/hwui/controls/Button.h"
#include <parameters/MacroControlParameter.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameters/MacroControlSmoothingParameter.h>
#include <proxies/hwui/HWUI.h>

bool MacroControlSmoothingParameterLayout::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  if(down)
  {
    switch(i)
    {
      case Buttons::BUTTON_B:
      {
        m_modResetActive = !m_modResetActive;
        highlightButtonWithCaption("Mod Reset", m_modResetActive);
        return true;
      }
      case Buttons::BUTTON_C:
      {
        selectMacroControl();
        return true;
      }
      case Buttons::BUTTON_ENTER:
      {
        if(m_modResetActive)
        {
          resetMod();
          return true;
        }
        else
        {
          break;
        }
      }
      default:
        break;
    }
  }

  return UnmodulateableParameterSelectLayout2::onButton(i, down, modifiers);
}

void MacroControlSmoothingParameterLayout::resetMod()
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  EditBufferUseCases ebUseCases { eb };

  if(auto modTime = dynamic_cast<MacroControlSmoothingParameter *>(eb->getSelected(getHWUI()->getCurrentVoiceGroup())))
  {
    if(auto mcUseCases = ebUseCases.getMCUseCase(modTime->getMC()))
    {
      mcUseCases->resetConnectionsToTargets();
      m_modResetActive = false;
      highlightButtonWithCaption("Mod Reset", false);
    }
  }
}

void MacroControlSmoothingParameterLayout::selectMacroControl()
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  EditBufferUseCases ebUseCases { eb };
  if(auto modTime = dynamic_cast<MacroControlSmoothingParameter *>(eb->getSelected(getHWUI()->getCurrentVoiceGroup())))
  {
    ebUseCases.selectParameter(modTime->getMC(), true);
  }
}

void MacroControlSmoothingParameterLayout::init()
{
  UnmodulateableParameterSelectLayout2::init();
  addControl(new Button("Mod Reset", Buttons::BUTTON_B));
  addControl(new Button("back..", Buttons::BUTTON_C));
}

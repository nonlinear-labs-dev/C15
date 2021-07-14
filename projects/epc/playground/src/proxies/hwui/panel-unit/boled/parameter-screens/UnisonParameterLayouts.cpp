#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/UnisonParameterCarousel.h>
#include "UnisonParameterLayouts.h"
#include <proxies/hwui/controls/Button.h>
#include <proxies/hwui/HWUI.h>
#include <parameters/mono-mode-parameters/ModulateableMonoParameter.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>

Carousel *UnmodulateableUnisonParameterLayout::createCarousel(const Rect &rect)
{
  return new UnisonParameterCarousel(rect);
}

void UnmodulateableUnisonParameterLayout::init()
{
  UnmodulateableParameterSelectLayout2::init();

  for(auto &c : getControls<Button>())
  {
    if(c->getButtonPos(Buttons::BUTTON_C) == c->getPosition())
    {
      c->setText(StringAndSuffix { "back..", 0 });
    }
  }
}

bool UnmodulateableUnisonParameterLayout::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  if(down && i == Buttons::BUTTON_C)
  {
    Application::get().getHWUI()->setFocusAndMode({ UIFocus::Sound, UIMode::Select, UIDetail::Voices });
    return true;
  }

  return ParameterSelectLayout2::onButton(i, down, modifiers);
}

Carousel *ModulateableUnisonParameterLayout::createCarousel(const Rect &rect)
{
  return new UnisonParameterCarousel(rect);
}

bool ModulateableUnisonParameterLayout::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  if(down && i == Buttons::BUTTON_C)
  {
    if(!isCurrentParameterModulated())
    {
      Application::get().getHWUI()->setFocusAndMode({ UIFocus::Sound, UIMode::Select, UIDetail::Voices });
      return true;
    }
  }

  return ModulateableParameterSelectLayout2::onButton(i, down, modifiers);
}

bool ModulateableUnisonParameterLayout::isCurrentParameterModulated() const
{
  auto selected
      = Application::get().getPresetManager()->getEditBuffer()->getSelected(getHWUI()->getCurrentVoiceGroup());
  if(auto mod = dynamic_cast<const ModulateableParameter *>(selected))
  {
    return mod->getModulationSource() != MacroControls::NONE;
  }
  return false;
}

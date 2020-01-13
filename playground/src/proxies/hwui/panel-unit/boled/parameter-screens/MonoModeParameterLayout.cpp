#include <Application.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MonoParameterCarousel.h>
#include "MonoModeParameterLayout.h"
#include <proxies/hwui/controls/Button.h>
#include <proxies/hwui/HWUI.h>
#include <parameters/mono-mode-parameters/ModulateableMonoParameter.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MonoUnisonModuleCaption.h>

Parameter *MonoModeParameterLayout::getCurrentParameter() const
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  return eb->getSelected();
}

ModuleCaption *MonoModeParameterLayout::createModuleCaption() const
{
  return new MonoUnisonModuleCaption(Rect(0, 0, 64, 13));
}

MonoModeParameterLayout::MonoModeParameterLayout()
    : UnmodulateableParameterSelectLayout2()
{
}

void MonoModeParameterLayout::init()
{
  UnmodulateableParameterSelectLayout2::init();

  for(auto &c : getControls<Button>())
  {
    if(c->getButtonPos(Buttons::BUTTON_C) == c->getPosition())
    {
      c->setText({ "back..", 0 });
    }
  }
}

Carousel *MonoModeParameterLayout::createCarousel(const Rect &rect)
{
  return new MonoParameterCarousel(rect);
}

bool MonoModeParameterLayout::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  if(down && i == Buttons::BUTTON_C)
  {
    Application::get().getHWUI()->setFocusAndMode({ UIFocus::Sound, UIMode::Select, UIDetail::Voices });
    return true;
  }

  return ParameterSelectLayout2::onButton(i, down, modifiers);
}

Parameter *MonoModeModulateableParameterLayout::getCurrentParameter() const
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  return eb->getSelected();
}

MonoModeModulateableParameterLayout::MonoModeModulateableParameterLayout()
    : ModulateableParameterSelectLayout2()
{
}

Carousel *MonoModeModulateableParameterLayout::createCarousel(const Rect &rect)
{
  return new MonoParameterCarousel(rect);
}

bool MonoModeModulateableParameterLayout::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  if(down && i == Buttons::BUTTON_C)
  {
    auto current = dynamic_cast<const ModulateableMonoParameter *>(
        Application::get().getPresetManager()->getEditBuffer()->getSelected());
    if(current && current->getModulationSource() == MacroControls::NONE)
    {
      Application::get().getHWUI()->setFocusAndMode({ UIFocus::Sound, UIMode::Select, UIDetail::Voices });
      return true;
    }
  }

  return ModulateableParameterSelectLayout2::onButton(i, down, modifiers);
}

ModuleCaption *MonoModeModulateableParameterLayout::createModuleCaption() const
{
  return new MonoUnisonModuleCaption(Rect(0, 0, 64, 13));
}

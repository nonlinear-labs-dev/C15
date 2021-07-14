#include <Application.h>
#include <parameters/Parameter.h>
#include <presets/EditBuffer.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/controls/Rect.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/boled/BOLED.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/PresetInfoLayout.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/BankInfoLayout.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/PresetManagerLayout.h>
#include <proxies/hwui/panel-unit/boled/setup/SetupLayout.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/SingleSoundLayout.h>
#include <proxies/hwui/panel-unit/boled/SplashLayout.h>
#include <proxies/hwui/panel-unit/boled/undo/UndoLayout.h>
#include <proxies/hwui/descriptive-layouts/LayoutFactory.h>
#include <proxies/hwui/descriptive-layouts/LayoutFolderMonitor.h>
#include <proxies/hwui/debug-oled/DebugLayout.h>
#include <tools/ExceptionTools.h>
#include <proxies/hwui/descriptive-layouts/GenericLayout.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ParameterCarousel.h>
#include <device-settings/Settings.h>
#include <device-settings/ScreenSaverTimeoutSetting.h>
#include "BOLEDScreenSaver.h"

BOLED::BOLED()
    : OLEDProxy(Rect(0, 0, 256, 64))
{
}

BOLED::~BOLED() = default;

void BOLED::init()
{
  reset(new SplashLayout());

  LayoutFolderMonitor::get().onChange(sigc::mem_fun(this, &BOLED::bruteForce));
  Application::get().getSettings()->getSetting<ScreenSaverTimeoutSetting>()->onScreenSaverStateChanged(
      sigc::mem_fun(this, &BOLED::toggleScreenSaver));
}

void BOLED::toggleScreenSaver(bool enabled)
{
  if(enabled)
  {
    installScreenSaver(new BOLEDScreenSaver(*this));
  }
  else if(dynamic_cast<BOLEDScreenSaver *>(getScreenSaver()) != nullptr)
  {
    removeScreenSaver();
  }
}

void BOLED::bruteForce()
{
  setupFocusAndMode(Application::get().getHWUI()->getFocusAndMode());
}

void BOLED::setupFocusAndMode(FocusAndMode focusAndMode)
{
  try
  {
    if(focusAndMode.mode == UIMode::Info && focusAndMode.focus == UIFocus::Sound)
      focusAndMode.mode = UIMode::Select;

    reset(DescriptiveLayouts::BoledLayoutFactory::get().instantiate(focusAndMode));
  }
  catch(...)
  {
    installOldLayouts(focusAndMode);
  }
}

void BOLED::installOldLayouts(FocusAndMode focusAndMode)
{
  switch(focusAndMode.focus)
  {
    case UIFocus::Parameters:
      setupParameterScreen(focusAndMode);
      break;

    case UIFocus::Presets:
      setupPresetScreen(focusAndMode);
      break;

    case UIFocus::Banks:
      setupBankScreen(focusAndMode);
      break;

    case UIFocus::Sound:
      setupSoundScreen(focusAndMode);
      break;

    case UIFocus::Setup:
      reset(new SetupLayout(focusAndMode));
      break;

    default:
      g_assert_not_reached();
      break;
  }
}

void BOLED::setupSoundScreen(FocusAndMode focusAndMode)
{
  reset(new SingleSoundLayout(focusAndMode));
}

void BOLED::setupParameterScreen(FocusAndMode focusAndMode)
{
  if(auto selParam = Application::get().getPresetManager()->getEditBuffer()->getSelected(
         Application::get().getHWUI()->getCurrentVoiceGroup()))
  {
    auto layout = selParam->createLayout(focusAndMode);

    if(auto currentLayout = getLayout().get())
    {
      auto descriptiveLayout = dynamic_cast<DescriptiveLayouts::GenericLayout *>(currentLayout) != nullptr;

      if(descriptiveLayout && currentLayout && typeid(*layout) == typeid(*currentLayout))
      {
        currentLayout->copyFrom(layout);
        delete layout;
      }
      else
      {
        reset(layout);
      }
    }
  }
}

void BOLED::setupPresetScreen(FocusAndMode focusAndMode)
{
  switch(focusAndMode.mode)
  {
    case UIMode::Info:
      reset(new PresetInfoLayout());
      break;

    default:
      reset(new PresetManagerLayout(focusAndMode, Application::get().getHWUI()->getOldFocusAndMode()));
  }
}

void BOLED::setupBankScreen(FocusAndMode focusAndMode)
{
  switch(focusAndMode.mode)
  {
    case UIMode::Info:
      reset(new BankInfoLayout());
      break;

    default:
      if(auto e = std::dynamic_pointer_cast<PresetManagerLayout>(getLayout()))
        e->setFocusAndMode(focusAndMode);
      else
        reset(new PresetManagerLayout(focusAndMode, Application::get().getHWUI()->getFocusAndMode()));
  }
}

bool BOLED::onButtonPressed(Buttons buttonID, ButtonModifiers modifiers, bool state)
{
  if(auto l = std::dynamic_pointer_cast<Layout>(getLayout()))
    if(l->onButton(buttonID, state, modifiers))
      return true;

  return false;
}

void BOLED::onRotary(signed char i)
{
  if(auto l = std::dynamic_pointer_cast<Layout>(getLayout()))
    l->onRotary(i, Application::get().getHWUI()->getButtonModifiers());
}

void BOLED::showUndoScreen()
{
  reset(new UndoLayout());
}
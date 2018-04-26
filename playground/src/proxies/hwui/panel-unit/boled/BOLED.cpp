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

BOLED::BOLED () :
    OLEDProxy (Rect (0, 0, 256, 64))
{
  
}

BOLED::~BOLED ()
{
}

void BOLED::init()
{
  reset(new SplashLayout());

  LayoutFolderMonitor::get().onChange(sigc::mem_fun(this, &BOLED::bruteForce));
}

void BOLED::bruteForce()
{
  setupFocusAndMode(Application::get().getHWUI()->getFocusAndMode());
}

void BOLED::setupFocusAndMode (FocusAndMode focusAndMode)
{
  if(auto layout = DescriptiveLayouts::BoledLayoutFactory::get().instantiate(focusAndMode))
  {
    reset (layout);
    return;
  }

  switch (focusAndMode.focus)
  {
    case UIFocus::Parameters:
      setupParameterScreen (focusAndMode);
      break;

    case UIFocus::Presets:
      setupPresetScreen (focusAndMode);
      break;

    case UIFocus::Banks:
      setupBankScreen (focusAndMode);
      break;

    case UIFocus::Sound:
      setupSoundScreen (focusAndMode);
      break;

    case UIFocus::Setup:
      reset (new SetupLayout (focusAndMode));
      break;

    default:
      g_assert_not_reached()
      ;
      break;
  }
}

void BOLED::setupSoundScreen (FocusAndMode focusAndMode)
{
  reset (new SingleSoundLayout (focusAndMode));
}

void BOLED::setupParameterScreen (FocusAndMode focusAndMode)
{
  if (auto selParam = Application::get ().getPresetManager ()->getEditBuffer ()->getSelected ())
  {
    auto layout = selParam->createLayout (focusAndMode);

    if (getLayout () && typeid(*layout) == typeid(*getLayout ().get ()))
    {
      getLayout ()->copyFrom (layout);
      delete layout;
    }
    else
    {
      reset (layout);
    }
  }
}

void BOLED::setupPresetScreen (FocusAndMode focusAndMode)
{
  switch (focusAndMode.mode)
  {
    case UIMode::Info:
      reset (new PresetInfoLayout ());
      break;

    default:
      if (auto e = dynamic_pointer_cast<PresetManagerLayout> (getLayout ()))
        e->setFocusAndMode (focusAndMode);
      else
        reset (new PresetManagerLayout (focusAndMode));
  }
}

void BOLED::setupBankScreen (FocusAndMode focusAndMode)
{
  switch (focusAndMode.mode)
  {
    case UIMode::Info:
      reset (new BankInfoLayout ());
      break;

    default:
      if (auto e = dynamic_pointer_cast<PresetManagerLayout> (getLayout ()))
        e->setFocusAndMode (focusAndMode);
      else
        reset (new PresetManagerLayout (focusAndMode));
  }
}

bool BOLED::onButtonPressed (gint32 buttonID, ButtonModifiers modifiers, bool state)
{
  if (shared_ptr<DFBLayout> l = dynamic_pointer_cast<DFBLayout> (getLayout ()))
    if (l->onButton (buttonID, state, modifiers))
      return true;

  return false;
}

void BOLED::onRotary (signed char i)
{
  if (shared_ptr<DFBLayout> l = dynamic_pointer_cast<DFBLayout> (getLayout ()))
    l->onRotary (i, Application::get ().getHWUI ()->getButtonModifiers ());
}

void BOLED::showUndoScreen ()
{
  reset (new UndoLayout ());
}

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
#include <proxies/hwui/descriptive-layouts/ConditionRegistry.h>
#include <device-settings/LayoutMode.h>
#include "BOLED.h"
#include <proxies/hwui/descriptive-layouts/GenericLayout.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ParameterCarousel.h>

BOLED::BOLED()
    : OLEDProxy(Rect(0, 0, 256, 64))
{
}

BOLED::~BOLED() = default;

void BOLED::init()
{
  reset(new SplashLayout());

  LayoutFolderMonitor::get().onChange(sigc::mem_fun(this, &BOLED::bruteForce));
}

void BOLED::bruteForce()
{
  setupFocusAndMode(Application::get().getHWUI()->getFocusAndMode());
}

void BOLED::setupFocusAndModeMixed(FocusAndMode focusAndMode)
{
  try
  {
    reset(DescriptiveLayouts::BoledLayoutFactory::get().instantiate(focusAndMode));
  }
  catch(...)
  {
    try
    {
      installOldLayouts(focusAndMode);
      return;
    }
    catch(...)
    {
      std::rethrow_exception(std::current_exception());
    }

    auto description = ExceptionTools::handle_eptr(std::current_exception());
    Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().reset(new DebugLayout(description));
  }
}

void BOLED::setupFocusAndModeDescriptiveLayouts(FocusAndMode focusAndMode)
{
  try
  {
    if(auto newLayout = DescriptiveLayouts::BoledLayoutFactory::get().instantiate(focusAndMode))
    {
      if(focusAndMode.focus == UIFocus::Parameters)
      {
        if(!isSameParameterScreen(dynamic_cast<const DescriptiveLayouts::GenericLayout*>(newLayout.get()),
                                  focusAndMode))
        {
          reset(newLayout);
        }
      }
      else
      {
        reset(newLayout);
      }
    }
    else
    {
      DebugLevel::throwException("No DescriptiveLayout for:", focusAndMode.toString(), "found!");
    }
    return;
  }
  catch(std::runtime_error& err)
  {
    DebugLevel::error(err.what());
  }
  catch(...)
  {
  }

  installOldLayouts(focusAndMode);
}

void BOLED::setupFocusAndMode(FocusAndMode focusAndMode)
{
  switch(Application::get().getSettings()->getSetting<LayoutMode>()->get())
  {
    case Old:
      installOldLayouts(focusAndMode);
      break;
    case Mixed:
      setupFocusAndModeMixed(focusAndMode);
      break;
    case New:
      setupFocusAndModeDescriptiveLayouts(focusAndMode);
      break;
  }
}

bool BOLED::isSameParameterScreen(const DescriptiveLayouts::GenericLayout* layout,
                                  const FocusAndMode& focusAndMode) const
{
  auto fam = layout->getPrototype().getDesiredFocusAndMode();

  auto currentInstalledGenericLayout = dynamic_cast<const DescriptiveLayouts::GenericLayout*>(getLayout().get());

  if(fam.focus == UIFocus::Parameters)
  {
    if(fam.mode == focusAndMode.mode && currentInstalledGenericLayout)
    {
      for(auto& controls : currentInstalledGenericLayout->getControls())
      {
        if(auto paramCarousel = dynamic_cast<const ParameterCarousel*>(controls.get()))
        {
          return paramCarousel->containsSelectedParameter();
        }
      }
    }
  }

  return false;
}

void BOLED::reset(Layout* layout)
{
  OLEDProxy::reset(layout);
  m_layoutInstantiated.emit(layout);
}

void BOLED::reset(OLEDProxy::tLayoutPtr layout)
{
  OLEDProxy::reset(layout);
  m_layoutInstantiated.emit(layout.get());
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
  if(auto selParam = Application::get().getPresetManager()->getEditBuffer()->getSelected())
  {
    auto layout = selParam->createLayout(focusAndMode);

    if(auto currentLayout = getLayout().get())
    {
      auto descriptiveLayout = dynamic_cast<DescriptiveLayouts::GenericLayout*>(currentLayout) != nullptr;

      if(descriptiveLayout && currentLayout && typeid(*layout) == typeid(*currentLayout))
      {
        currentLayout->copyFrom(layout);
        delete layout;
      }
      else if(!descriptiveLayout)
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
      if(auto e = std::dynamic_pointer_cast<PresetManagerLayout>(getLayout()))
        e->setFocusAndMode(focusAndMode);
      else
        reset(new PresetManagerLayout(focusAndMode));
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
        reset(new PresetManagerLayout(focusAndMode));
  }
}

bool BOLED::onButtonPressed(Buttons buttonID, ButtonModifiers modifiers, bool state)
{
  if(auto l = std::dynamic_pointer_cast<DFBLayout>(getLayout()))
    if(l->onButton(buttonID, state, modifiers))
      return true;

  return false;
}

void BOLED::onRotary(signed char i)
{
  if(auto l = std::dynamic_pointer_cast<DFBLayout>(getLayout()))
    l->onRotary(i, Application::get().getHWUI()->getButtonModifiers());
}

void BOLED::runPerformanceTest()
{
  Application::get().stopWatchDog();

  for(auto& versionMode : { LayoutVersionMode::Old, LayoutVersionMode::New, LayoutVersionMode::Mixed })
  {
    Application::get().getSettings()->getSetting<LayoutMode>()->set(versionMode);

    auto totalTries = 0;
    auto totalValue = 0.0;

    auto start = std::chrono::high_resolution_clock::now();
    for(auto& focus : { UIFocus::Parameters, UIFocus::Presets, UIFocus::Sound, UIFocus::Banks, UIFocus::Setup })
    {
      for(auto& mode : { UIMode::Select, UIMode::Edit, UIMode::Info, UIMode::Store })
      {
        for(auto& detail : { UIDetail::ButtonA, UIDetail::ButtonB, UIDetail::ButtonC, UIDetail::ButtonD, UIDetail::Init,
                             UIDetail::MCAmount, UIDetail::MCModRange, UIDetail::MCPosition, UIDetail::MCSelect })
        {
          auto start2 = std::chrono::high_resolution_clock::now();
          setupFocusAndMode({ focus, mode, detail });
          auto end2 = std::chrono::high_resolution_clock::now();
          totalTries++;
          totalValue += std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2).count();
        }
      }
    }

    std::cout << "Average over " << totalTries << " instantiations: " << totalValue / totalTries << "ms\n";

    auto end = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    auto str = Application::get().getSettings()->getSetting<LayoutMode>()->getDisplayString();
    std::cerr << str << " full FocusAndMode traversal took : " << diff.count() << " ms " << std::endl;
  }
  Application::get().runWatchDog();
}

void BOLED::showUndoScreen()
{
  reset(new UndoLayout());
}

sigc::connection BOLED::onLayoutInstantiated(const sigc::slot<void, Layout*>& s)
{
  return m_layoutInstantiated.connect(s);
}

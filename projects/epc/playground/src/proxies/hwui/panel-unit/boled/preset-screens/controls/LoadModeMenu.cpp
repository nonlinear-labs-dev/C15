#include "LoadModeMenu.h"
#include "LoadToPartPresetList.h"
#include "use-cases/DirectLoadUseCases.h"
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <device-settings/DirectLoadSetting.h>
#include <proxies/hwui/controls/Label.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/panel-unit/boled/BOLED.h>
#include <device-settings/Settings.h>
#include <device-settings/DirectLoadSetting.h>
#include <sigc++/sigc++.h>
#include <proxies/hwui/FrameBuffer.h>
#include <proxies/hwui/controls/Button.h>

auto getDirectLoadSetting()
{
  return Application::get().getSettings()->getSetting<DirectLoadSetting>();
}

bool LoadModeMenu::redraw(FrameBuffer& fb)
{
  return ControlWithChildren::redraw(fb);
}

LoadModeMenu::LoadModeMenu(const Rect& rect)
    : ControlWithChildren(rect)
    , m_toggleDirectLoadAction { [this]
                  {
                    DirectLoadUseCases useCase(getDirectLoadSetting().get());
                    auto hwui = Application::get().getHWUI();
                    useCase.toggleDirectLoadFromHWUI(hwui);
                  } }
{
  m_soundTypeConnection = Application::get().getPresetManager()->getEditBuffer()->onSoundTypeChanged(
      sigc::hide(sigc::mem_fun(this, &LoadModeMenu::bruteForce)));

  m_directLoadSettingConnection
      = getDirectLoadSetting()->onChange(sigc::hide(sigc::mem_fun(this, &LoadModeMenu::bruteForce)));

  m_loadToPartConnection = Application::get().getHWUI()->onLoadToPartModeChanged(
      sigc::hide(sigc::mem_fun(this, &LoadModeMenu::bruteForce)));
}

LoadModeMenu::~LoadModeMenu()
{
  m_soundTypeConnection.disconnect();
  m_directLoadSettingConnection.disconnect();
}

void LoadModeMenu::bruteForce()
{
  clear();

  switch(getSoundType())
  {
    default:
    case SoundType::Single:
      installSingle();
      break;
    case SoundType::Layer:
    case SoundType::Split:
      installDual();
      break;
  }
}

void toggleLoadToPartDetail()
{
  auto hwui = Application::get().getHWUI();
  auto pm = Application::get().getPresetManager();

  if(pm->getNumBanks() != 0)
  {
    hwui->toggleLoadToPart();
  }
}

void LoadModeMenu::installSingle()
{
  m_buttonDHandler = std::make_unique<ShortVsLongPress>(m_toggleDirectLoadAction, m_toggleDirectLoadAction);

  auto directLoadButton = addControl(new Button("Direct Load", { 0, 15, 58, 11 }));
  directLoadButton->setHighlight(isDirectLoadEnabled());
}

void LoadModeMenu::setBackgroundColor(FrameBuffer& fb) const
{
  fb.setColor(FrameBufferColors::C43);
}

void LoadModeMenu::installDual()
{
  m_buttonDHandler = std::make_unique<ShortVsLongPress>([this] { toggleLoadToPartDetail(); }, m_toggleDirectLoadAction);

  auto loadToPartButton = addControl(new Button("To Part", { 0, 15, 58, 11 }));
  loadToPartButton->setHighlight(isLoadToPartEnabled());

  auto directLoadButton = addControl(new Button("Direct Ld", { 4, 0, 50, 11 }));
  directLoadButton->setHighlight(isDirectLoadEnabled());
}

SoundType LoadModeMenu::getSoundType()
{
  return Application::get().getPresetManager()->getEditBuffer()->getType();
}

bool LoadModeMenu::isDirectLoadEnabled()
{
  return Application::get().getSettings()->getSetting<DirectLoadSetting>()->get();
}

bool LoadModeMenu::isLoadToPartEnabled()
{
  return Application::get().getHWUI()->isInLoadToPart();
}

bool LoadModeMenu::onButton(Buttons button, bool down, ButtonModifiers modifiers)
{
  if(m_buttonDHandler && button == Buttons::BUTTON_D)
  {
    m_buttonDHandler->onButtonEvent(down);
    return true;
  }

  return false;
}

#include "LoadModeMenu.h"
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
{
  m_soundTypeConnection = Application::get().getPresetManager()->getEditBuffer()->onSoundTypeChanged(
      sigc::mem_fun(this, &LoadModeMenu::bruteForce));

  m_directLoadSettingConnection
      = getDirectLoadSetting()->onChange(sigc::hide(sigc::mem_fun(this, &LoadModeMenu::bruteForce)));
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
  auto focusAndMode = hwui->getFocusAndMode();
  if(focusAndMode.detail != UIDetail::LoadToPart)
  {
    hwui->setFocusAndMode(UIDetail::LoadToPart);
  }
  else
  {
    hwui->setFocusAndMode(UIDetail::Init);
  }
}

void LoadModeMenu::installSingle()
{
  m_buttonDHandler = std::make_unique<ShortVsLongPress>([this] { getDirectLoadSetting()->toggle(); },
                                                        [this] { getDirectLoadSetting()->toggle(); });

  auto directLoadButton = addControl(new Button("Direct Load", { 0, 50, 58, 11 }));
  directLoadButton->setHighlight(isDirectLoadEnabled());
}

void LoadModeMenu::installDual()
{
  m_buttonDHandler = std::make_unique<ShortVsLongPress>([this] { toggleLoadToPartDetail(); },
                                                        [this] { getDirectLoadSetting()->toggle(); });

  auto loadToPartButton = addControl(new Button("To Part", { 0, 50, 58, 11 }));
  loadToPartButton->setHighlight(isLoadToPartEnabled());

  auto directLoadButton = addControl(new Button("Direct Ld", { 4, 35, 50, 11 }));
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
  auto hwui = Application::get().getHWUI();
  return hwui->getFocusAndMode().detail == UIDetail::LoadToPart;
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

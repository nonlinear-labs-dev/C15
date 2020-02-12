#include "LoadModeMenu.h"
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <device-settings/LoadModeSetting.h>
#include <proxies/hwui/controls/Label.h>
#include <proxies/hwui/HWUI.h>

auto getSetting()
{
  return Application::get().getSettings()->getSetting<LoadModeSetting>();
}

LoadModeMenu::LoadModeMenu(const Rect& rect)
    : ControlWithChildren(rect)
{
  m_soundTypeConnection = Application::get().getPresetManager()->getEditBuffer()->onSoundTypeChanged(
      sigc::mem_fun(this, &LoadModeMenu::bruteForce));

  m_settingConnection = getSetting()->onChange(sigc::hide(sigc::mem_fun(this, &LoadModeMenu::bruteForce)));
}

LoadModeMenu::~LoadModeMenu()
{
  m_soundTypeConnection.disconnect();
  m_settingConnection.disconnect();
}

class EntryLabel : public Label
{
 public:
  using Label::Label;

  void drawBackground(FrameBuffer& fb) override
  {
    const Rect& r = getPosition();

    if(isHighlight())
    {
      fb.setColor(FrameBuffer::Colors::C128);
      fb.fillRect(r.getX() + 1, r.getTop() + 1, r.getWidth() - 2, r.getHeight() - 2);
    }
    else
    {
      Label::drawBackground(fb);
    }
  }
};

void LoadModeMenu::bruteForce()
{
  clear();

  std::vector<LoadMode> entries;

  if(getSoundType() != SoundType::Single)
    entries = { LoadMode::LoadToPart, LoadMode::Select, LoadMode::DirectLoad };
  else
    entries = { LoadMode::Select, LoadMode::DirectLoad };

  const auto numItems = entries.size();
  const auto buttonHeight = 13;
  auto y = getPosition().getBottom() - numItems * (buttonHeight - 1) - 2;

  const auto& loadModeSetting = getSetting();
  const auto& loadModeDisplayStrings = loadModeSetting->enumToDisplayString();

  const auto& currentMode = loadModeSetting->get();
  const auto currentVoiceGroup = Application::get().getHWUI()->getCurrentVoiceGroup();

  for(const auto& entry : entries)
  {
    const auto text = loadModeSetting->getDisplayStringForVoiceGroup(currentVoiceGroup, entry);
    auto newControl = addControl(new EntryLabel({ text, 0 }, Rect(0, y, getWidth(), buttonHeight)));

    if(entry == currentMode)
      newControl->setHighlight(true);

    y += (buttonHeight - 1);
  }
}

void LoadModeMenu::turn()
{
  getSetting()->cycleForSoundType(getSoundType());
}

void LoadModeMenu::antiTurn()
{
  getSetting()->antiCycleForSoundType(getSoundType());
}

SoundType LoadModeMenu::getSoundType()
{
  return Application::get().getPresetManager()->getEditBuffer()->getType();
}

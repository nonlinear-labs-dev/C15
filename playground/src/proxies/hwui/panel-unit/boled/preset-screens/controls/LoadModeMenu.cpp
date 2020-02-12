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
  std::vector<LoadMode> entries;

  if(getSoundType() != SoundType::Single)
  {
    entries = { LoadMode::LoadToPart, LoadMode::Select, LoadMode::DirectLoad };
  }
  else
  {
    entries = { LoadMode::Select, LoadMode::DirectLoad };
  }

  clear();

  auto numItems = entries.size();
  const auto buttonHeight = 13;
  auto y = getPosition().getBottom() - numItems * (buttonHeight - 1) - 2;

  const auto& setting = getSetting();
  const auto& displayStrings = setting->enumToDisplayString();

  const auto& currentMode = setting->get();
  const auto currentVg = Application::get().getHWUI()->getCurrentVoiceGroup();

  for(const auto& entry : entries)
  {
    auto text = setting->getDisplayStringForVoiceGroup(currentVg, entry);

    auto c = addControl(new EntryLabel({ text, 0 }, Rect(0, y, getWidth(), buttonHeight)));
    y += (buttonHeight - 1);

    if(entry == currentMode)
      c->setHighlight(true);
  }
}

void LoadModeMenu::turn()
{
  getSetting()->cycleForSoundType(getSoundType());
  bruteForce();
}

void LoadModeMenu::antiTurn()
{
  getSetting()->cycleForSoundType(getSoundType());
  bruteForce();
}

SoundType LoadModeMenu::getSoundType()
{
  return Application::get().getPresetManager()->getEditBuffer()->getType();
}

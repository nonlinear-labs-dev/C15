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
  bool redraw(FrameBuffer& fb) override
  {
    auto oldColor = fb.getColor();

    if(isHighlight())
      fb.setColor(FrameBuffer::C128);
    else
      fb.setColor(FrameBuffer::C103);

    fb.fillRect(getPosition());

    fb.setColor(oldColor);

    return Label::redraw(fb);
  }
};

void LoadModeMenu::bruteForce()
{
  m_entries.clear();

  if(getSoundType() != SoundType::Single)
  {
    m_entries[0] = LoadMode::LoadToPart;
    m_entries[1] = LoadMode::Select;
    m_entries[2] = LoadMode::DirectLoad;
  }
  else
  {
    m_entries[0] = LoadMode::Select;
    m_entries[1] = LoadMode::DirectLoad;
  }

  clear();

  auto y = 0;
  auto numItems = m_entries.size();
  auto wastedSpace = getHeight() - numItems * 13;
  y = wastedSpace;

  const auto& setting = getSetting();
  const auto& displayStrings = setting->enumToDisplayString();

  const auto& current = setting->get();
  const auto currentVg = Application::get().getHWUI()->getCurrentVoiceGroup();

  for(const auto& entry : m_entries)
  {
    auto text = setting->getDisplayStringForVoiceGroup(currentVg, entry.second);

    auto c = addControl(new EntryLabel({ text, 0 }, Rect(0, y, getWidth(), 13)));
    y += 13;
    if(entry.second == current)
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

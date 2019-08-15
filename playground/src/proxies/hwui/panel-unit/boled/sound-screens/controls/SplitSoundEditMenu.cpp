#include "SplitSoundEditMenu.h"
#include "Application.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include "tools/SingeltonShortcuts.h"
#include "proxies/hwui/descriptive-layouts/EventSink.h"

#include "proxies/hwui/HWUI.h"

SplitSoundEditMenu::SplitSoundEditMenu(const Rect& r)
    : SingleSoundEditMenu(r)
{
  init();
}

void SplitSoundEditMenu::init()
{

  auto* pm = Application::get().getPresetManager();

  clear();

  addButton("Import Preset into VG", [=]() {
    Application::get().getHWUI()->setFocusAndMode(UIDetail::SoundSelectPresetForVoiceGroup);
  });

  addButton("Convert to Single", [=]() {
    pm->getEditBuffer()->setType(Type::Single);
    Application::get().getHWUI()->setFocusAndMode(UIMode::Select);
    SiSc::HWUI::bruteForce();
  });

  selectButton(0);
}

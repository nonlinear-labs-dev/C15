#include "LayerSoundEditMenu.h"
#include "Application.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include "tools/SingeltonShortcuts.h"

#include "proxies/hwui/HWUI.h"

LayerSoundEditMenu::LayerSoundEditMenu(const Rect& rect)
    : SingleSoundEditMenu(rect)
{
  init();
}

void LayerSoundEditMenu::init()
{
  auto* pm = Application::get().getPresetManager();

  clear();
  addButton("Import Preset into VG", [=]() {
    //FOO;
  });

  addButton("Convert to Single", [=]() {
    pm->getEditBuffer()->setType(Type::Single);
    Application::get().getHWUI()->setFocusAndMode(UIMode::Select);
    SiSc::HWUI::bruteForce();
  });

  selectButton(0);
}

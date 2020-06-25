#include <Application.h>
#include "InitSound.h"
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/HWUI.h>
#include <libundo/undo/Scope.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/SoundEditInfoLayout.h>

InitSound::InitSound(const Rect& rect)
    : AnimatedGenericItem("Init Sound", rect, [] {
      auto pm = Application::get().getPresetManager();
      auto scope = pm->getUndoScope().startTransaction("Init Sound");
      pm->getEditBuffer()->undoableInitSound(scope->getTransaction());
      auto hwui = Application::get().getHWUI();
      hwui->undoableSetFocusAndMode(scope->getTransaction(), { UIFocus::Sound, UIMode::Select, UIDetail::Init });
    })
{
}

Control* InitSound::createInfo()
{
  return new SoundEditInfoLayout(SetupInfoEntries::InitSound);
}

InitPart::InitPart(const Rect& r)
    : AnimatedGenericItem("Init Part", r, []() {
      auto pm = Application::get().getPresetManager();
      auto scope = pm->getUndoScope().startTransaction("Init Part");
      auto sel = Application::get().getHWUI()->getCurrentVoiceGroup();
      pm->getEditBuffer()->undoableInitPart(scope->getTransaction(), sel);
      auto hwui = Application::get().getHWUI();
      hwui->setFocusAndMode({ UIFocus::Sound, UIMode::Select, UIDetail::Init });
    })
{
}

Control* InitPart::createInfo()
{
  return new SoundEditInfoLayout(SetupInfoEntries::InitPart);
}

#include <Application.h>
#include "InitSound.h"
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/HWUI.h>
#include <libundo/undo/Scope.h>

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

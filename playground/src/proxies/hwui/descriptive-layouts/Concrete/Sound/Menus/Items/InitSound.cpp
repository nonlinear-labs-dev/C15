#include <Application.h>
#include "InitSound.h"
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>

InitSound::InitSound()
    : AnimatedGenericItem("Init Sound", [] {
      auto pm = Application::get().getPresetManager();
      auto scope = pm->getUndoScope().startTransaction("Init Sound");
      pm->getEditBuffer()->undoableInitSound(scope->getTransaction());
    })
{
}

#include "RandomizeOverlay.h"
#include <Application.h>
#include <device-settings/RandomizeAmount.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/descriptive-layouts/Concrete/Menu/SettingLabel.h>

RandomizeOverlay::RandomizeOverlay(const Rect& r)
    : ChangeSettingWithCommitOverlay<RandomizeAmount>(r, []{
      auto scope = Application::get().getPresetManager()->getUndoScope().startTransaction("Randomize");
      Application::get().getPresetManager()->getEditBuffer()->undoableRandomize(scope->getTransaction(),
                                                                                Initiator::EXPLICIT_HWUI);
    })
{
}
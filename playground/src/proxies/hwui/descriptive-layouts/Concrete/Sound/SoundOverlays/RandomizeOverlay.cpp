#include "RandomizeOverlay.h"
#include <Application.h>
#include <device-settings/RandomizeAmount.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/descriptive-layouts/Concrete/Menu/SettingLabel.h>

RandomizeOverlay::RandomizeOverlay(const Rect& r)
    : ArrowIncrementDecrementOverlay(r)
{
  auto labelWidth = r.getWidth() - 20;
  addControl(new SettingLabel<RandomizeAmount>({11, -1, labelWidth, 13}));
}

RandomizeOverlay::~RandomizeOverlay()
= default;

RandomizeAmount* RandomizeOverlay::getSetting()
{
  return Application::get().getSettings()->getSetting<RandomizeAmount>().get();
}

void RandomizeOverlay::onLeft(bool down)
{
  if(down)
  {
    getSetting()->incDec(-1, {});
  }
}

void RandomizeOverlay::onRight(bool down)
{
  if(down)
  {
    getSetting()->incDec(1, {});
  }
}

void RandomizeOverlay::onCommit(bool down)
{
  if(down)
  {
    auto scope = Application::get().getPresetManager()->getUndoScope().startTransaction("Randomize");
    Application::get().getPresetManager()->getEditBuffer()->undoableRandomize(scope->getTransaction(),
                                                                              Initiator::EXPLICIT_HWUI);
  }
}

#include "RandomizeItem.h"
#include <Application.h>
#include <device-settings/RandomizeAmount.h>
#include <proxies/hwui/descriptive-layouts/concrete/menu/SettingLabel.h>

#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>

#include <libundo/undo/Scope.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/descriptive-layouts/concrete/menu/menu-overlays/ChangeSettingWithCommitOverlay.h>
#include <proxies/hwui/panel-unit/boled/setup/SetupInfoTexts.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/SoundEditInfoLayout.h>

RandomizeItem::RandomizeItem(const Rect& rect)
    : EditorItem("Randomize Sound", rect)
{
  auto rightHalf = getPosition();
  rightHalf.setWidth(rightHalf.getWidth() / 2);
  rightHalf.setLeft(rightHalf.getWidth());

  addControl(new SettingLabel<RandomizeAmount>(rightHalf));
}

Control* RandomizeItem::createInfo()
{
  return new SoundEditInfoLayout(SetupInfoEntries::RandomizeSound);
}

RandomizeItem::~RandomizeItem() = default;

MenuOverlay* RandomizeItem::createOverlay()
{
  return new ChangeSettingWithCommitOverlay<RandomizeAmount>(getDefaultOverlayRect(), [] {
    auto scope = Application::get().getPresetManager()->getUndoScope().startTransaction("Randomize Sound");
    Application::get().getPresetManager()->getEditBuffer()->undoableRandomize(scope->getTransaction(),
                                                                              Initiator::EXPLICIT_HWUI);
  });
}

RandomizePart::RandomizePart(const Rect& r)
    : EditorItem("Randomize Part", r)
{
  auto rightHalf = getPosition();
  rightHalf.setWidth(rightHalf.getWidth() / 2);
  rightHalf.setLeft(rightHalf.getWidth());

  addControl(new SettingLabel<RandomizeAmount>(rightHalf));
}

Control* RandomizePart::createInfo()
{
  return new SoundEditInfoLayout(SetupInfoEntries::RandomizePart);
}

RandomizePart::~RandomizePart() = default;

MenuOverlay* RandomizePart::createOverlay()
{
  return new ChangeSettingWithCommitOverlay<RandomizeAmount>(getDefaultOverlayRect(), [] {
    auto vg = Application::get().getHWUI()->getCurrentVoiceGroup();
    auto currentVoiceGroup = toString(vg);
    auto scope = Application::get().getPresetManager()->getUndoScope().startTransaction("Randomize Part %s",
                                                                                        currentVoiceGroup);
    Application::get().getPresetManager()->getEditBuffer()->undoableRandomizePart(scope->getTransaction(), vg,
                                                                                  Initiator::EXPLICIT_HWUI);
  });
}

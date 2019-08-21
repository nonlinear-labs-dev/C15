#include "RandomizeItem.h"
#include <Application.h>
#include <device-settings/RandomizeAmount.h>
#include <proxies/hwui/descriptive-layouts/Concrete/Menu/SettingLabel.h>

RandomizeEditor::RandomizeEditor()
    : EditorItem("Randomize")
{
  auto rightHalf = getPosition();
  rightHalf.setWidth(rightHalf.getWidth() / 2);
  rightHalf.setLeft(rightHalf.getWidth());

  addControl(new SettingLabel<RandomizeAmount>(rightHalf));
}

RandomizeEditor::~RandomizeEditor()
= default;

ScrollMenuOverlay* RandomizeEditor::createOverlay()
{
  return new RandomizeOverlay(getStandartOverlayRect());
}
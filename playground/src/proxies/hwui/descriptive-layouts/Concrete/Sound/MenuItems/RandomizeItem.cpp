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
  auto half = getPosition();
  half.setHeight(half.getHeight() - 2);
  half.setTop(getPosition().getTop() + 1);
  half.setWidth(half.getWidth() / 2);
  half.setLeft(half.getWidth());

  return new RandomizeOverlay(half);
}

void RandomizeEditor::doAction() {
}

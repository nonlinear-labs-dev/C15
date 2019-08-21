#include "RandomizeItem.h"
#include <Application.h>
#include <device-settings/RandomizeAmount.h>
#include <proxies/hwui/descriptive-layouts/Concrete/Menu/SettingLabel.h>

RandomizeItem::RandomizeItem()
    : EditorItem("Randomize")
{
  auto rightHalf = getPosition();
  rightHalf.setWidth(rightHalf.getWidth() / 2);
  rightHalf.setLeft(rightHalf.getWidth());

  addControl(new SettingLabel<RandomizeAmount>(rightHalf));
}

RandomizeItem::~RandomizeItem()
= default;

ScrollMenuOverlay* RandomizeItem::createOverlay()
{
  return new RandomizeOverlay(getStandartOverlayRect());
}
#include "EditorItem.h"

bool EditorItem::canEnter() {
  return true;
}

Rect EditorItem::getStandartOverlayRect() {
  auto half = getPosition();
  half.setHeight(half.getHeight() - 2);
  half.setTop(half.getTop() + 1);
  half.setWidth((half.getWidth() / 2) - 2);
  half.setLeft(getPosition().getWidth() / 2);
  return half;
}

void EditorItem::doAction() {
}

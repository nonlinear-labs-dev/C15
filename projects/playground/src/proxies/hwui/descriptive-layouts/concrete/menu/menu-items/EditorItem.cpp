#include <proxies/hwui/controls/ArrowLeft.h>
#include "EditorItem.h"
#include <proxies/hwui/FrameBuffer.h>
#include <proxies/hwui/Oleds.h>

bool EditorItem::canEnter()
{
  return true;
}

Rect EditorItem::getDefaultOverlayRect()
{
  auto half = getPosition();
  half.setHeight(half.getHeight() - 2);
  half.setTop(half.getTop() + 1);
  half.setWidth((half.getWidth() / 2) - 2);
  half.setLeft(getPosition().getWidth() / 2);
  return half;
}

Rect EditorItem::getFullRightSideOverlayRect()
{
  auto rightHalf = getPosition();
  rightHalf.setHeight(52);
  rightHalf.setTop(0);
  rightHalf.setWidth(rightHalf.getWidth() / 2);
  rightHalf.setLeft(getPosition().getWidth() / 2);
  return rightHalf;
}

void EditorItem::doAction()
{
}

bool EditorItem::redraw(FrameBuffer &fb)
{
  auto ret = BasicItem::redraw(fb);
  ret |= redrawEnterIndication(fb);
  return ret;
}

bool EditorItem::redrawEnterIndication(FrameBuffer &buffer)
{
  auto leftRect = getPosition();
  leftRect.setWidth(12);
  leftRect.setLeft(getPosition().getRight() - 12);
  auto f = Oleds::get().getFont("Emphase-9-Regular", 9);
  buffer.setColor(isHighlight() ? FrameBufferColors::C255 : FrameBufferColors::C128);
  f->draw("...", leftRect.getCenter().getX() - 4, leftRect.getBottom() - 1);
  return true;
}

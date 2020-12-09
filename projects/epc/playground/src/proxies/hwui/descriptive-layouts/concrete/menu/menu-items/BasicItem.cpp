#include "BasicItem.h"
#include "LeftAligned9pxCaptionLabel.h"
#include <proxies/hwui/controls/LeftAlignedLabel.h>
#include <proxies/hwui/descriptive-layouts/concrete/menu/menu-items/controls/CaptionLabel.h>
#include <proxies/hwui/FrameBuffer.h>
#include <proxies/hwui/Oleds.h>

bool BasicItem::canEnter()
{
  return false;
}

bool BasicItem::redraw(FrameBuffer& fb)
{
  auto ret = false;
  ret |= ControlWithChildren::redraw(fb);
  ret |= drawHighlightBorder(fb);
  return ret;
}

bool BasicItem::drawHighlightBorder(FrameBuffer& fb)
{
  if(!isHighlight())
    return false;

  fb.setColor(FrameBufferColors::C179);
  fb.drawRect(getPosition());
  return true;
}

bool BasicItem::drawEnterIndication(FrameBuffer& buffer)
{
  auto pos = getPosition();
  pos.setWidth(12);
  pos.setLeft(getPosition().getRight() - 12);
  pos.setHeight(pos.getHeight() - 1);
  auto f = Oleds::get().getFont("Emphase-9-Regular", 9);
  buffer.setColor(isHighlight() ? FrameBufferColors::C255 : FrameBufferColors::C128);
  f->draw("\u23CE", pos.getCenter().getX() - 4, pos.getBottom() - 1);
  return true;
}

void BasicItem::init()
{
  auto leftHalf = getPosition();
  leftHalf.setTop(leftHalf.getTop() + 1);
  leftHalf.setLeft(1);
  leftHalf.setWidth((leftHalf.getWidth() / 2) - 1);

  addControl(new CaptionLabel<LeftAligned9pxCaptionLabel>(m_caption, leftHalf, true, true));
}

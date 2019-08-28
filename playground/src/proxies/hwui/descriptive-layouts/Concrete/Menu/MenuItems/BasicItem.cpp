#include "BasicItem.h"
#include "AnimationControl.h"

bool BasicItem::canEnter()
{
  return false;
}

bool BasicItem::redraw(FrameBuffer& fb)
{
  auto ret = false;
  ret |= ControlWithChildren::redraw(fb);
  ret |= drawAnimation(fb);
  ret |= drawHighlightBorder(fb);
  return ret;
}

bool BasicItem::drawHighlightBorder(FrameBuffer& fb)
{
  if(!isHighlight())
    return false;

  fb.setColor(FrameBuffer::C103);
  fb.drawRect(getPosition());
  return true;
}

void BasicItem::addAnimation(AnimationControl *animationControl) {
  if(m_animationControl) {
    remove(m_animationControl);
  }

  m_animationControl = addControl(animationControl);
  m_animationControl->startAnimation([]{});
}

void BasicItem::resetAnimation() {
  if(m_animationControl) {
    remove(m_animationControl);
    m_animationControl = nullptr;
  }
}

bool BasicItem::drawAnimation(FrameBuffer &fb) {
  if(m_animationControl)
    return m_animationControl->redraw(fb);
  return false;
}

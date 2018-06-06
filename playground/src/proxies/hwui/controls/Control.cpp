#include "Control.h"

Control::Control (const Rect &pos) :
    m_rect (pos),
    m_isDirty (true),
    m_highlight (false),
    m_visible (true)
{
}

Control::~Control ()
{
}

const Rect &Control::getPosition () const
{
  return m_rect;
}

void Control::setPosition (const Rect &rect)
{
  if(m_rect != rect)
  {
    m_rect = rect;
    setDirty ();
  }
}

void Control::setDirty ()
{
  if (!m_isDirty)
  {
    m_isDirty = true;
  }
}

void Control::setClean ()
{
  if (m_isDirty)
  {
    m_isDirty = false;
  }
}

bool Control::isVisible () const
{
  return m_visible;
}

void Control::setVisible (bool b)
{
  if (m_visible != b)
  {
    m_visible = b;
    setDirty ();
  }
}

bool Control::isDirty () const
{
  if(m_isDirty)
  {
    return m_isDirty;
  }
  return m_isDirty;
}

void Control::setHighlight (bool isHighlight)
{
  if (m_highlight != isHighlight)
  {
    m_highlight = isHighlight;
    setDirty ();
  }
}

bool Control::isHighlight () const
{
  return m_highlight;
}

bool Control::redraw (FrameBuffer &fb)
{
  return false;
}

void Control::setBackgroundColor (FrameBuffer &fb) const
{
  fb.setColor (FrameBuffer::Colors::C43);
}

void Control::drawBackground (FrameBuffer &fb)
{
  setBackgroundColor (fb);
  fb.fillRect (getPosition());
}

int Control::getHeight() const
{
  return m_rect.getHeight();
}

void Control::collectDirtyRects(std::list<Rect> &rects) const
{
  if(isDirty())
    rects.push_back(getPosition());
}

void Control::setDirtyIfOverlapsWithAny(const std::list<Rect> &rects)
{
  if(!isDirty())
    if(overlapsWithAny(rects))
      setDirty();
}

bool Control::overlapsWithAny(const std::list<Rect> &rects) const
{
  for(auto &r : rects)
    if(r.intersects(getPosition()))
      return true;

  return false;
}


#include "Control.h"
#include "proxies/hwui/Oleds.h"

Control::Control(const Rect &pos)
    : m_rect(pos)
    , m_isDirty(true)
    , m_highlight(false)
    , m_visible(true)
{
}

Control::~Control()
{
}

const Rect &Control::getPosition() const
{
  return m_rect;
}

void Control::setPosition(const Rect &rect)
{
  m_rect = rect;
  setDirty();
}

void Control::setDirty()
{
  m_isDirty = true;
  Oleds::get().setDirty();
}

void Control::setClean()
{
  m_isDirty = false;
}

bool Control::isVisible() const
{
  return m_visible;
}

void Control::setVisible(bool b)
{
  if(m_visible != b)
  {
    m_visible = b;
    setDirty();
  }
}

bool Control::isDirty() const
{
  return m_isDirty;
}

void Control::setHighlight(bool isHighlight)
{
  if(m_highlight != isHighlight)
  {
    m_highlight = isHighlight;
    setDirty();
  }
}

bool Control::isHighlight() const
{
  return m_highlight;
}

bool Control::redraw(FrameBuffer &fb)
{
  return false;
}

void Control::setBackgroundColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBuffer::Colors::C43);
}

void Control::drawBackground(FrameBuffer &fb)
{
  setBackgroundColor(fb);
  fb.fillRect(getPosition());
}

int Control::getHeight() const
{
  return m_rect.getHeight();
}

int Control::getWidth() const
{
  return m_rect.getWidth();
}

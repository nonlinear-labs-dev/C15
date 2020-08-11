#include "Control.h"
#include "proxies/hwui/Oleds.h"
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/FrameBuffer.h>
#include <Application.h>

Control::Control(const Rect &pos)
    : m_rect(pos)
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
  Oleds::get().setDirty();
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
  fb.setColor(FrameBufferColors::C43);
}

int Control::getZPriority() const
{
  return 0;
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

HWUI *Control::getHWUI() const
{
  return Application::get().getHWUI();
}
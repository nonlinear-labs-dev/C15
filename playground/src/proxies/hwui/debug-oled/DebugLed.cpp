#include "DebugLed.h"

DebugLed::DebugLed(const Rect &pos, int led)
    : super(pos)
    , m_ledId(led)
    , m_onOrOff(false)
{
}

DebugLed::~DebugLed()
{
}

void DebugLed::setLedState(bool onOrOff)
{
  if(m_onOrOff != onOrOff)
  {
    m_onOrOff = onOrOff;
    setDirty();
  }
}

int DebugLed::getId() const
{
  return m_ledId;
}

bool DebugLed::redraw(FrameBuffer &fb)
{
  super::redraw(fb);

  Rect r = getPosition();
  r.reduceByMargin(2);

  if(m_onOrOff)
    fb.setColor(FrameBuffer::Colors::C255);
  else
    fb.setColor(FrameBuffer::Colors::C77);

  fb.fillRect(r.getLeft(), r.getTop(), r.getWidth(), r.getHeight());
  return true;
}

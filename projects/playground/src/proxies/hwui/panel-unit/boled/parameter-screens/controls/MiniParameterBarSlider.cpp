#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MiniParameterBarSlider.h>
#include <proxies/hwui/FrameBuffer.h>

MiniParameterBarSlider::MiniParameterBarSlider(Parameter *param, const Rect &rect)
    : super(param, rect)
{
}

bool MiniParameterBarSlider::hasBorder() const
{
  return false;
}

bool MiniParameterBarSlider::redraw(FrameBuffer &fb)
{
  Rect r = getPosition();

  if(isHighlight())
    fb.setColor(FrameBufferColors::C128);
  else
    fb.setColor(FrameBufferColors::C77);

  fb.fillRect(r.getX(), r.getTop(), r.getWidth(), r.getHeight());
  super::redraw(fb);

  return true;
}

bool MiniParameterBarSlider::drawCenterMark() const
{
  return false;
}
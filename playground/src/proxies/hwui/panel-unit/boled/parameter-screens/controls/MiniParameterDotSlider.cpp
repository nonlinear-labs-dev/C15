#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MiniParameterDotSlider.h>

MiniParameterDotSlider::MiniParameterDotSlider(Parameter *param, const Rect &rect)
    : super(param, rect)
{
}

MiniParameterDotSlider::~MiniParameterDotSlider()
{
}

bool MiniParameterDotSlider::hasBorder() const
{
  return false;
}

bool MiniParameterDotSlider::redraw(FrameBuffer &fb)
{
  Rect r = getPosition();

  if(isHighlight())
    fb.setColor(FrameBuffer::Colors::C128);
  else
    fb.setColor(FrameBuffer::Colors::C103);

  fb.fillRect(r.getX(), r.getTop(), r.getWidth(), r.getHeight());
  super::redraw(fb);

  return true;
}

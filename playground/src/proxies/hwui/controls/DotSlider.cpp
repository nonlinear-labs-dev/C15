#include "DotSlider.h"

static const int c_borderLeft = 1;
static const int c_borderRight = 1;
static const int c_handleWidth = 4;
static const int c_smallHandleWidth = 2;

DotSlider::DotSlider(Parameter *param, const Rect &rect)
    : super(param, rect)
{
}

DotSlider::DotSlider(const Rect &rect)
    : super(rect)
{
}

DotSlider::~DotSlider()
{
}

bool DotSlider::redraw(FrameBuffer &fb)
{
  drawSliderBackground(fb);

  if(getParameter())
    drawHandle(fb);

  return true;
}

void DotSlider::drawSliderBackground(FrameBuffer &fb)
{
  const Rect &r = getPosition();
  fb.setColor(FrameBuffer::Colors::C128);
  fb.drawRect(r.getLeft(), r.getTop(), r.getWidth(), r.getHeight());
}

void DotSlider::drawHandle(FrameBuffer &fb)
{
  const Rect &r = getPosition();
  const int handleHeight = r.getHeight();
  auto halfHandle = c_handleWidth / 2;

  int x = getHandlePixelPosition();

  if(!isHighlight())
  {
    fb.setColor(FrameBuffer::Colors::C103);
    fb.fillRect(x - halfHandle, r.getTop(), c_handleWidth, handleHeight);
    fb.setColor(FrameBuffer::Colors::C179);
  }
  else
  {
    fb.setColor(FrameBuffer::Colors::C255);
  }

  if(hasBorder())
  {
    fb.fillRect(x + 1 - halfHandle, r.getTop(), c_handleWidth - 2, handleHeight);
    fb.fillRect(x - halfHandle, r.getTop() + 1, c_handleWidth, handleHeight - 2);
    fb.fillRect(x - 1 - halfHandle, r.getTop() + 2, c_handleWidth + 2, handleHeight - 4);
  }
  else
  {
    fb.fillRect(x - c_smallHandleWidth / 2, r.getTop(), c_smallHandleWidth, handleHeight);
    fb.fillRect(x - c_smallHandleWidth / 2, r.getTop() + 1, c_smallHandleWidth, handleHeight);
  }
}

int DotSlider::getHandlePixelPosition()
{
  Rect r = getPosition();
  int usableWidth = 0;

  if(hasBorder())
    usableWidth = r.getWidth() - c_borderLeft - c_borderRight - c_handleWidth;
  else
    usableWidth = r.getWidth() - c_smallHandleWidth;

  if(isBiPolar())
  {
    auto v = max(min(getValue(), 1.0), -1.0);
    return r.getCenter().getX() + ceil(v * usableWidth * 0.5);
  }
  else
  {
    auto v = max(min(getValue(), 1.0), 0.0);
    if(hasBorder())
      return r.getLeft() + c_borderLeft + c_handleWidth / 2 + ceil(v * usableWidth);
    else
      return r.getLeft() + c_smallHandleWidth / 2 + ceil(v * usableWidth);
  }
}

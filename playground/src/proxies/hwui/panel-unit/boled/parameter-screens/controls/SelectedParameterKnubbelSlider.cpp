#include "SelectedParameterKnubbelSlider.h"

static const int c_borderLeft = 1;
static const int c_borderRight = 1;
static const int c_handleWidth = 4;

SelectedParameterKnubbelSlider::SelectedParameterKnubbelSlider(const Rect &rect)
    : super(rect)
{
}

SelectedParameterKnubbelSlider::~SelectedParameterKnubbelSlider()
{
}

void SelectedParameterKnubbelSlider::drawHandle(FrameBuffer &fb)
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

  fb.fillRect(x + 1 - halfHandle, r.getTop(), c_handleWidth - 2, handleHeight);
  fb.fillRect(x - halfHandle, r.getTop() + 1, c_handleWidth, handleHeight - 2);
}

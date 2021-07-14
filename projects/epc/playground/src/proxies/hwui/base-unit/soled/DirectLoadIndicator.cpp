#include <proxies/hwui/base-unit/soled/DirectLoadIndicator.h>
#include <proxies/hwui/FrameBuffer.h>

DirectLoadIndicator::DirectLoadIndicator(const Rect &pos)
    : base_type(pos)
{
}

DirectLoadIndicator::~DirectLoadIndicator()
{
}

void DirectLoadIndicator::setMode(Mode m)
{
  if(m_mode != m)
  {
    m_mode = m;
    setText(StringAndSuffix { m_mode == Mode::DirectLoad ? "DL" : "" });
    setDirty();
  }
}

bool DirectLoadIndicator::redraw(FrameBuffer &fb)
{
  base_type::redraw(fb);

  switch(m_mode)
  {
    case Mode::Off:
      break;

    case Mode::DirectLoad:
      drawDirectLoad(fb);
      break;

    case Mode::Load:
      drawArrow(fb);
      break;
  }
  return true;
}

void DirectLoadIndicator::drawDirectLoad(FrameBuffer &fb)
{
  auto pos = getPosition();
  pos.addMargin(5, 1, 7, 2);
  fb.drawRect(pos);
}

void DirectLoadIndicator::drawArrow(FrameBuffer &fb) const
{
  auto pos = getPosition();
  pos.addMargin(9, 3, 2, 3);

  auto xCenter = pos.getCenter().getX();

  fb.drawHorizontalLine(pos.getLeft() + 1, pos.getBottom(), xCenter - pos.getLeft() - 1);
  fb.drawVerticalLine(xCenter, pos.getTop(), pos.getHeight() - 1);

  fb.setPixel(xCenter, pos.getTop() + 1);
  for(int xy = 1; xy <= 2; xy++)
  {
    fb.setPixel(xCenter - xy, pos.getTop() + xy);
    fb.setPixel(xCenter + xy, pos.getTop() + xy);
  }
}

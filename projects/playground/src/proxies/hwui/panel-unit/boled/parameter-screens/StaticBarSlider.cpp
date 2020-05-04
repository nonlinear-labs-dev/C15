#include <cmath>
#include "StaticBarSlider.h"
#include "proxies/hwui/FrameBuffer.h"

StaticBarSlider::StaticBarSlider(tControlPositionValue v, bool isBiPolar, const Rect& pos)
    : BarSlider(nullptr, pos)
{
  setHighlight(true);
  setValue(v, isBiPolar);
}

void StaticDrawFromRightBarSlider::drawParameterSlider(FrameBuffer& fb, double v, const Rect& smaller)
{
  v = std::max(std::min(v, 1.0), 0.0);
  auto width = std::ceil(smaller.getWidth() * v);
  auto realWidth = smaller.getWidth() - width;
  setSliderColor(fb);
  fb.fillRect(smaller.getRight() - realWidth, smaller.getTop(), realWidth + 1, smaller.getHeight());
}

StaticDrawFromRightBarSlider::StaticDrawFromRightBarSlider(tControlPositionValue v, const Rect& r)
    : BarSlider(r)
{
  setHighlight(true);
  setValue(v, false);
}

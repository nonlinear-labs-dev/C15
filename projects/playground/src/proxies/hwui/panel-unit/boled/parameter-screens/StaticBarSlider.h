#pragma once

#include <proxies/hwui/controls/BarSlider.h>

class StaticBarSlider : public BarSlider
{
 public:
  StaticBarSlider(tControlPositionValue v, bool isBiPolar, const Rect& pos);
};

class StaticDrawFromRightBarSlider : public BarSlider
{
 public:
  StaticDrawFromRightBarSlider(tControlPositionValue v, const Rect& r);

 protected:
  void drawParameterSlider(FrameBuffer& fb, double v, const Rect& smaller) override;
};

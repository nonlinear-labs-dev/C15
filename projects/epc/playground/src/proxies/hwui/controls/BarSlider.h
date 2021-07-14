#pragma once

#include "Slider.h"

class BarSlider : public Slider
{
 private:
  typedef Slider super;

 public:
  BarSlider(Parameter *param, const Rect &rect);
  explicit BarSlider(const Rect &rect);

  bool redraw(FrameBuffer &fb) override;
  virtual tDisplayValue getDrawValue() const;

 protected:
  virtual bool drawCenterMark() const;
  virtual void drawParameterSlider(FrameBuffer &fb, double v, const Rect &smaller);
  virtual void drawParameterSliderFromRight(FrameBuffer &fb, double v, const Rect &smaller);
};

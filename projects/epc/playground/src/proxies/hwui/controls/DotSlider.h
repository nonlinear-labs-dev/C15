#pragma once

#include "Slider.h"

class DotSlider : public Slider
{
 private:
  typedef Slider super;

 public:
  DotSlider(Parameter *param, const Rect &rect);
  DotSlider(const Rect &rect);

  bool redraw(FrameBuffer &fb);

 protected:
  virtual void drawSliderBackground(FrameBuffer &fb);
  virtual void drawHandle(FrameBuffer &fb);
  int getHandlePixelPosition();
};

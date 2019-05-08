#pragma once

#include "Slider.h"

class BarSlider : public Slider
{
 private:
  typedef Slider super;

 public:
  BarSlider(Parameter *param, const Rect &rect);
  BarSlider(const Rect &rect);
  virtual ~BarSlider();

  bool redraw(FrameBuffer &fb) override;

 protected:
  virtual bool drawCenterMark() const;

 private:
  // private methods
  BarSlider(const BarSlider &other);
  BarSlider &operator=(const BarSlider &);
};

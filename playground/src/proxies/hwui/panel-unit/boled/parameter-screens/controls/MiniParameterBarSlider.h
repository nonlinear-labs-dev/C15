#pragma once

#include "proxies/hwui/controls/BarSlider.h"

class MiniParameterBarSlider : public BarSlider
{
 private:
  typedef BarSlider super;

 public:
  // public methods
  MiniParameterBarSlider(Parameter* param, const Rect& rect);
  virtual ~MiniParameterBarSlider();

  bool redraw(FrameBuffer& fb) override;

 private:
  // private methods
  MiniParameterBarSlider(const MiniParameterBarSlider& other);
  MiniParameterBarSlider& operator=(const MiniParameterBarSlider&);

  bool hasBorder() const override;
  bool drawCenterMark() const override;
};

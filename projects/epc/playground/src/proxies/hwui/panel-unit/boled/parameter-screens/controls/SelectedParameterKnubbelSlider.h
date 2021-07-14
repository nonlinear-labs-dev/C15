#pragma once

#include "SelectedParameterDotSlider.h"

class SelectedParameterKnubbelSlider : public SelectedParameterDotSlider
{
 private:
  typedef SelectedParameterDotSlider super;

 public:
  explicit SelectedParameterKnubbelSlider(const Rect &rect);

 protected:
  void drawHandle(FrameBuffer &fb) override;
};

#pragma once

#include "SelectedParameterDotSlider.h"

class PhysicalControlSlider : public SelectedParameterDotSlider
{
 private:
  typedef SelectedParameterDotSlider super;

 public:
  using super::super;

 protected:
  void drawSliderBackground(FrameBuffer &fb) override;
};

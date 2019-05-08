#pragma once

#include "SelectedParameterDotSlider.h"

class PhysicalControlSlider : public SelectedParameterDotSlider
{
 private:
  typedef SelectedParameterDotSlider super;

 public:
  using super::super;

 protected:
  virtual void drawSliderBackground(FrameBuffer &fb) override;
};

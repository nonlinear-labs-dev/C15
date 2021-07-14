#pragma once

#include <proxies/hwui/controls/ControlWithChildren.h>

class Parameter;

class PedalSelectionControl : public ControlWithChildren
{
  typedef ControlWithChildren super;

 public:
  explicit PedalSelectionControl(Parameter *param);
  ~PedalSelectionControl() override;

  void setPosition(const Rect &rect) override;

 protected:
  void setBackgroundColor(FrameBuffer &fb) const override;
};

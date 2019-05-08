#pragma once

#include <proxies/hwui/controls/ControlWithChildren.h>

class Parameter;

class PedalSelectionControl : public ControlWithChildren
{
  typedef ControlWithChildren super;

 public:
  PedalSelectionControl(Parameter *param);
  virtual ~PedalSelectionControl();

  void setPosition(const Rect &rect) override;
};

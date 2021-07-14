#pragma once

#include <proxies/hwui/controls/ControlWithChildren.h>

class Overlay : public ControlWithChildren
{
 public:
  typedef ControlWithChildren super;

  explicit Overlay(const Rect &pos);

  using super::addControl;
  using super::clear;

  void setBackgroundColor(FrameBuffer &fb) const override;
  void drawBackground(FrameBuffer &fb) override;
  bool isTransparent() const override;
};

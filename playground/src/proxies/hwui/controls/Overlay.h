#pragma once

#include <proxies/hwui/controls/ControlWithChildren.h>

class Overlay : public ControlWithChildren
{
 public:
  typedef ControlWithChildren super;

  Overlay(const Rect &pos);

  using super::addControl;
  using super::clear;

  virtual void setBackgroundColor(FrameBuffer &fb) const override;
  virtual void drawBackground(FrameBuffer &fb) override;
  virtual bool isTransparent() const override;
};

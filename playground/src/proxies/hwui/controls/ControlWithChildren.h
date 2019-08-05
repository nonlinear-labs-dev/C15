#pragma once

#include "Control.h"
#include "ControlOwner.h"

class ControlWithChildren : public Control, public ControlOwner
{
 private:
  typedef Control super;

 public:
  explicit ControlWithChildren(const Rect &pos);
  ~ControlWithChildren() override;

  bool redraw(FrameBuffer &fb) override;
  void drawBackground(FrameBuffer &fb) override;

  bool isDirty() const override;
  void setDirty() override;

  void setHighlight(bool isHighlight) override;
  bool isHighlight() const override;

 protected:
  virtual bool isTransparent() const;
};

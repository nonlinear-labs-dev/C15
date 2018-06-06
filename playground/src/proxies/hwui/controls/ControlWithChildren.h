#pragma once

#include "Control.h"
#include "ControlOwner.h"

class ControlWithChildren : public Control, public ControlOwner
{
  private:
    typedef Control super;

  public:
    ControlWithChildren (const Rect &pos);
    virtual ~ControlWithChildren ();

    virtual bool isDirty () const  override;
    bool redraw (FrameBuffer &fb) override;
    void drawBackground (FrameBuffer &fb) override;
    virtual bool isHighlight() const override;
    virtual void setDirty() override;
    void collectDirtyRects(std::list<Rect> &rects) const override;
    void setDirtyIfOverlapsWithAny(const std::list<Rect> &rects) override;

    void setHighlight (bool isHighlight) override;

  protected:
    virtual bool isTransparent() const;
};


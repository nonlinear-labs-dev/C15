#pragma once
#include <proxies/hwui/controls/ControlWithChildren.h>
#include <proxies/hwui/controls/LeftAlignedLabel.h>

class AnimationControl;

class BasicItem : public ControlWithChildren
{
 protected:
  class CaptionLabel : public LeftAlignedLabel
  {
   public:
    CaptionLabel(const Glib::ustring& caption, const Rect& rect)
        : LeftAlignedLabel(caption, rect)
    {

    }

  protected:
    int getXOffset() const override {
      return 2;
    }

  protected:
    void setBackgroundColor(FrameBuffer &fb) const override {
      if(isHighlight())
        fb.setColor(FrameBuffer::C128);
      else
        fb.setColor(FrameBuffer::C43);
    }
  };

 public:
  template <class T>
  explicit BasicItem(T caption)
      : ControlWithChildren({ 0, 0, 254, 13 })
      , m_caption{ caption }
  {
    auto leftHalf = getPosition();
    leftHalf.setLeft(leftHalf.getLeft() + 1);
    leftHalf.setWidth((leftHalf.getWidth() / 2) - 1);

    m_captionLabel = addControl(new CaptionLabel(m_caption, leftHalf));
  }

  bool redraw(FrameBuffer& fb) override;

  virtual bool canEnter();
  virtual void doAction() = 0;

protected:
  void addAnimation(AnimationControl* animationControl);
  void resetAnimation();

  bool drawAnimation(FrameBuffer& fb);
  bool drawHighlightBorder(FrameBuffer& fb);

  Glib::ustring m_caption;
  LeftAlignedLabel* m_captionLabel = nullptr;

  AnimationControl* m_animationControl = nullptr;
};

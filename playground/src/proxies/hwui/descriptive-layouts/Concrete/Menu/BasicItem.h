#pragma once
#include <proxies/hwui/controls/ControlWithChildren.h>
#include <proxies/hwui/controls/LeftAlignedLabel.h>

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
    void setBackgroundColor(FrameBuffer &fb) const override {
      if(isHighlight())
        fb.setColor(FrameBuffer::C128);
      else
        fb.setColor(FrameBuffer::C43);
    }
  };

 public:
  template <class T>
  BasicItem(T caption, int orderNumber = 0)
      : ControlWithChildren({ 0, 0, 254, 13 })
      , m_caption{ caption }
      , m_orderNumber{ orderNumber }
  {
    auto leftHalf = getPosition();
    leftHalf.setLeft(getPosition().getLeft() + 1);
    leftHalf.setWidth(leftHalf.getWidth() / 2 - 2);

    m_captionLabel = addControl(new CaptionLabel(m_caption, leftHalf));
  }

  bool redraw(FrameBuffer& fb) override;

  virtual bool canEnter();
  virtual void doAction() = 0;

  int getOrderNumber() const;

 protected:
  bool drawHighlightBorder(FrameBuffer& fb);

  int m_orderNumber;
  Glib::ustring m_caption;
  LeftAlignedLabel* m_captionLabel;
};

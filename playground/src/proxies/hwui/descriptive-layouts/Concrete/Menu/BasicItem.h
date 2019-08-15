#pragma once
#include <proxies/hwui/controls/ControlWithChildren.h>
#include <proxies/hwui/controls/LeftAlignedLabel.h>

class BasicItem : public ControlWithChildren
{
 public:
  template <class T>
  BasicItem(T caption, int orderNumber = 0)
      : ControlWithChildren({0, 0, 254, 15})
      , m_caption{ caption }
      , m_orderNumber{orderNumber}
  {
    auto leftHalf = getPosition();
    leftHalf.setWidth(leftHalf.getWidth() / 2);

    m_captionLabel = addControl(new LeftAlignedLabel(m_caption, leftHalf));
  }

  void redraw(FrameBuffer& fb, int yOffset);

  virtual bool canEnter();
  virtual void doAction() = 0;

  int getOrderNumber() const;

 protected:
  int m_orderNumber;
  Glib::ustring m_caption;
  LeftAlignedLabel* m_captionLabel;
};

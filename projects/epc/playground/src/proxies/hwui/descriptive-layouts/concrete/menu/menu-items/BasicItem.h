#pragma once

#include "MenuItem.h"

class BasicItem : public MenuItem
{
 public:
  template <class T>
  explicit BasicItem(T caption, const Rect& r)
      : MenuItem(r)
      , m_caption { caption }
  {
    init();
  }

  bool redraw(FrameBuffer& fb) override;
  bool canEnter() override;

 protected:
  virtual bool drawHighlightBorder(FrameBuffer& fb);
  bool drawEnterIndication(FrameBuffer& buffer);

 private:
  void init();

  Glib::ustring m_caption;
};

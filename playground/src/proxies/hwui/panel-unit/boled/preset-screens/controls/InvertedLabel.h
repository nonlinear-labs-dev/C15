#pragma once

#include "proxies/hwui/controls/Label.h"

class InvertedLabel : public Label
{
 private:
  typedef Label super;

 public:
  InvertedLabel(const Glib::ustring& text, const Rect& pos);
  InvertedLabel(const Rect& pos);
  ~InvertedLabel();

  bool redraw(FrameBuffer& fb) override;
};

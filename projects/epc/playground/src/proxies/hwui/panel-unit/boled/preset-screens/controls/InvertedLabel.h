#pragma once

#include "proxies/hwui/controls/Label.h"

class InvertedLabel : public Label
{
 private:
  typedef Label super;

 public:
  InvertedLabel(const Glib::ustring& text, const Rect& pos);
  explicit InvertedLabel(const Rect& pos);
  ~InvertedLabel() override;

  bool redraw(FrameBuffer& fb) override;
};

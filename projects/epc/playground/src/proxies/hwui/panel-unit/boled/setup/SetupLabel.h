#pragma once

#include <proxies/hwui/controls/Label.h>

class SetupLabel : public Label
{
 public:
  SetupLabel(const Glib::ustring &text, const Rect &pos);
  explicit SetupLabel(const Rect &pos);
  ~SetupLabel() override;

  Font::Justification getJustification() const override;
  bool redraw(FrameBuffer &fb) override;
  int getXOffset() const override;
};

#pragma once

#include <proxies/hwui/controls/LabelRegular8.h>

class FrameBuffer;
class Rect;

class SoledHeader : public LabelRegular8
{
 private:
  typedef LabelRegular8 base_type;

 public:
  SoledHeader(const Glib::ustring &text, const Rect &pos);
  ~SoledHeader() override;

 private:
  void setFontColor(FrameBuffer &fb) const override;
  bool redraw(FrameBuffer &fb) override;
  int getXOffset() const override;
};

#pragma once

#include <proxies/hwui/controls/Label.h>

class FileNameHeadlineLabel : public Label
{
 private:
  typedef Label super;

 public:
  FileNameHeadlineLabel(const Glib::ustring &str, const Rect &pos);
  bool redraw(FrameBuffer &fb) override;
  std::shared_ptr<Font> getFont() const override;
};
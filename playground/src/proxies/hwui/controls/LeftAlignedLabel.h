#pragma once

#include <proxies/hwui/controls/Label.h>

class LeftAlignedLabel : public Label
{
 public:
  LeftAlignedLabel(const Glib::ustring &text, const Rect &pos);

  virtual shared_ptr<Font> getFont() const;
  virtual int getFontHeight() const;
  virtual Font::Justification getJustification() const;
};

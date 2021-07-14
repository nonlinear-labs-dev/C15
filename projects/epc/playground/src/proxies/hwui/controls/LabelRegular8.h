#pragma once

#include "Label.h"

class LabelRegular8 : public Label
{
 private:
  typedef Label super;

 public:
  LabelRegular8(const Glib::ustring &text, const Rect &pos);
  explicit LabelRegular8(const Rect &pos);
  ~LabelRegular8() override;

 protected:
  std::shared_ptr<Font> getFont() const override;
  int getFontHeight() const override;

 private:
  LabelRegular8(const LabelRegular8 &other);
  LabelRegular8 &operator=(const LabelRegular8 &);
};

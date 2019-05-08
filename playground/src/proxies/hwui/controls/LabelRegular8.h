#pragma once

#include "Label.h"

class LabelRegular8 : public Label
{
 private:
  typedef Label super;

 public:
  LabelRegular8(const Glib::ustring &text, const Rect &pos);
  LabelRegular8(const Rect &pos);
  virtual ~LabelRegular8();

 protected:
  virtual shared_ptr<Font> getFont() const override;
  virtual int getFontHeight() const override;

 private:
  // private methods
  LabelRegular8(const LabelRegular8 &other);
  LabelRegular8 &operator=(const LabelRegular8 &);
};

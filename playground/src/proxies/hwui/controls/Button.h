#pragma once

#include "LabelRegular8.h"
#include <proxies/hwui/buttons.h>

class Application;

class Button : public LabelRegular8
{
 private:
  typedef LabelRegular8 super;

 public:
  Button(const Glib::ustring &caption, Buttons buttonID);
  Button(const Glib::ustring &caption, const Rect &rect);
  virtual ~Button();

  bool redraw(FrameBuffer &fb) override;
  void blind(bool b);

  static Rect getButtonPos(Buttons n);

 protected:
  void setFontColor(FrameBuffer &fb) const override;

 private:
  bool m_blind = false;
};

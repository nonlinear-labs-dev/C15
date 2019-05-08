#pragma once

#include "Label.h"

class ButtonMenuButton : public Label
{
 private:
  typedef Label super;

 public:
  ButtonMenuButton(bool isFirst, bool isLast, const Glib::ustring &text, const Rect &pos);
  virtual ~ButtonMenuButton();

  void drawBackground(FrameBuffer &fb);

 private:
  // private methods
  ButtonMenuButton(const ButtonMenuButton &other);
  ButtonMenuButton &operator=(const ButtonMenuButton &);

  bool m_isFirst;
  bool m_isLast;
};

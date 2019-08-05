#pragma once

#include "Label.h"

class ButtonMenuButton : public Label
{
 private:
  typedef Label super;

 public:
  ButtonMenuButton(bool isFirst, bool isLast, const Glib::ustring &text, const Rect &pos);
  ~ButtonMenuButton() override;

  void drawBackground(FrameBuffer &fb) override;
  void setJustification(Font::Justification justi);

 protected:
  Font::Justification getJustification() const override;

 private:
  // private methods
  ButtonMenuButton(const ButtonMenuButton &other);
  ButtonMenuButton &operator=(const ButtonMenuButton &);

  bool m_isFirst;
  bool m_isLast;
  Font::Justification m_justification;
};

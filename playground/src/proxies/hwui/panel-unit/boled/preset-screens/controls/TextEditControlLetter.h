#pragma once
#include <proxies/hwui/controls/Label.h>

class TextEditUsageMode;

class TextEditControlLetter : public Label
{
 private:
  typedef Label super;

 public:
  TextEditControlLetter(shared_ptr<TextEditUsageMode> textUsageMode, int relativeToCursor, const Rect &pos);

  virtual bool redraw(FrameBuffer &fb);
  virtual Glib::ustring getText() const;

 private:
  shared_ptr<TextEditUsageMode> m_textUsageMode;
  int m_relativeToCursor;
};

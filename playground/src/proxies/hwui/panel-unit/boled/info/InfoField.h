#pragma once

#include <proxies/hwui/HWUIEnums.h>

class SingleLineInfoContent;
class Control;

class InfoField
{
 public:
  InfoField(SingleLineInfoContent* label, Control* c);

  void setInfo(const Glib::ustring& text);
  void setInfo(const Glib::ustring& text, FrameBufferColors c);

  void setPosition(int y);
  int format(int y);

 private:
  SingleLineInfoContent* m_label;
  Control* m_content;
};

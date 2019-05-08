#pragma once

#include "proxies/hwui/controls/Label.h"

class Application;
class Setting;

class NoteShiftDisplay : public Label
{
 private:
  typedef Label super;

 public:
  NoteShiftDisplay(const Rect& rect);
  virtual ~NoteShiftDisplay();

 private:
  NoteShiftDisplay(const NoteShiftDisplay& other) = delete;
  NoteShiftDisplay& operator=(const NoteShiftDisplay&) = delete;

  void setDisplayText(const Setting* setting);
  Glib::ustring formatShift(int i) const;
};

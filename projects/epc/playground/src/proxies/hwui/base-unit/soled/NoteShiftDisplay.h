#pragma once

#include "proxies/hwui/controls/Label.h"

class Application;
class Setting;

class NoteShiftDisplay : public Label
{
 private:
  typedef Label super;

 public:
  explicit NoteShiftDisplay(const Rect& rect);
  ~NoteShiftDisplay() override;
  NoteShiftDisplay(const NoteShiftDisplay& other) = delete;
  NoteShiftDisplay& operator=(const NoteShiftDisplay&) = delete;

 private:
  void setDisplayText(const Setting* setting);
  Glib::ustring formatShift(int i) const;
};

#include "NoteShiftDisplay.h"
#include "Application.h"
#include "device-settings/Settings.h"
#include "device-settings/NoteShift.h"

NoteShiftDisplay::NoteShiftDisplay(const Rect& rect)
    : super(rect)
{
  Application::get().getSettings()->getSetting<NoteShift>()->onChange(mem_fun(this, &NoteShiftDisplay::setDisplayText));
}

NoteShiftDisplay::~NoteShiftDisplay() = default;

void NoteShiftDisplay::setDisplayText(const Setting* setting)
{
  if(auto n = dynamic_cast<const NoteShift*>(setting))
    setText(StringAndSuffix { formatShift(n->get()) });
}

Glib::ustring NoteShiftDisplay::formatShift(int i) const
{
  if(i == 0)
    return "-";

  char txt[50];
  sprintf(txt, "%+d", i);
  return txt;
}

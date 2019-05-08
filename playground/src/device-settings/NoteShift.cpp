#include "NoteShift.h"
#include "Application.h"
#include "device-settings/Settings.h"
#include "proxies/lpc/LPCProxy.h"
#include "http/UpdateDocumentMaster.h"
#include <device-settings/DebugLevel.h>

static const uint16_t c_NoteShiftSettingKey = NOTE_SHIFT;

NoteShift::NoteShift(Settings &parent)
    : super(parent)
    , m_shift(0)
{
}

NoteShift::~NoteShift()
{
}

void NoteShift::load(const Glib::ustring &text)
{
  try
  {
    set(stoi(text));
  }
  catch(...)
  {
    set(0);
    DebugLevel::error("Could not read settings for note shift:", text);
  }
}

Glib::ustring NoteShift::save() const
{
  return to_string(get());
}

void NoteShift::set(int shift)
{
  shift = std::max(-48, std::min(shift, 48));

  if(m_shift != shift)
  {
    m_shift = shift;
    notify();
  }
}

void NoteShift::sendToLPC() const
{
  uint16_t value = (m_shift >= 0) ? 0 : 0x8000;
  value |= abs(m_shift);
  Application::get().getLPCProxy()->sendSetting(c_NoteShiftSettingKey, value);
}

int NoteShift::get() const
{
  return m_shift;
}

void NoteShift::incSemiTone()
{
  set(get() + 1);
}

void NoteShift::decSemiTone()
{
  set(get() - 1);
}

void NoteShift::incOctave()
{
  set(get() + 12);
}

void NoteShift::decOctave()
{
  set(get() - 12);
}

#include "MidiReceiveAftertouchCurveSetting.h"

MidiReceiveAftertouchCurveSetting::MidiReceiveAftertouchCurveSetting(UpdateDocumentContributor& s)
    : EnumSetting<AftertouchCurves>(s, AftertouchCurves::AFTERTOUCH_CURVE_NORMAL)
{
}

const std::vector<Glib::ustring>& MidiReceiveAftertouchCurveSetting::enumToString() const
{
  static std::vector<Glib::ustring> s_modeNames = { "soft", "normal", "hard" };
  return s_modeNames;
}

const std::vector<Glib::ustring>& MidiReceiveAftertouchCurveSetting::enumToDisplayString() const
{
  static std::vector<Glib::ustring> s_modeNames = { "Soft", "Normal", "Hard" };
  return s_modeNames;
}

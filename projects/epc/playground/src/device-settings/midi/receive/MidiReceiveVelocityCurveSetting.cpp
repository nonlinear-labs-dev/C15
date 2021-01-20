#include "MidiReceiveVelocityCurveSetting.h"

MidiReceiveVelocityCurveSetting::MidiReceiveVelocityCurveSetting(UpdateDocumentContributor &s)
    : EnumSetting<VelocityCurves>(s, VelocityCurves::VELOCITY_CURVE_NORMAL)
{
}

const std::vector<Glib::ustring> &MidiReceiveVelocityCurveSetting::enumToString() const
{
  static std::vector<Glib::ustring> s_modeNames = { "very_soft", "soft", "normal", "hard", "very_hard" };
  return s_modeNames;
}

const std::vector<Glib::ustring> &MidiReceiveVelocityCurveSetting::enumToDisplayString() const
{
  static std::vector<Glib::ustring> s_modeNames = { "Very Soft", "Soft", "Normal", "Hard", "Very Hard" };
  return s_modeNames;
}
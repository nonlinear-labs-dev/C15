#include "BooleanSetting.h"

BooleanSetting::BooleanSetting(Settings &settings, bool def)
    : super(settings, def ? BooleanSettings::BOOLEAN_SETTING_TRUE : BooleanSettings::BOOLEAN_SETTING_FALSE)
{
}

BooleanSetting::~BooleanSetting()
{
}

bool BooleanSetting::get() const
{
  return super::get() == BooleanSettings::BOOLEAN_SETTING_TRUE;
}

void BooleanSetting::toggle()
{
  set(get() ? BooleanSettings::BOOLEAN_SETTING_FALSE : BooleanSettings::BOOLEAN_SETTING_TRUE);
}

const vector<Glib::ustring> &BooleanSetting::enumToString() const
{
  static vector<Glib::ustring> s_modeNames = { "on", "off" };
  return s_modeNames;
}

const vector<Glib::ustring> &BooleanSetting::enumToDisplayString() const
{
  static vector<Glib::ustring> s_modeNames = { "On", "Off" };
  return s_modeNames;
}

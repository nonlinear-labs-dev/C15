#include "BooleanSetting.h"

BooleanSetting::BooleanSetting(UpdateDocumentContributor &settings, bool def)
    : super(settings, def ? BooleanSettings::BOOLEAN_SETTING_TRUE : BooleanSettings::BOOLEAN_SETTING_FALSE)
{
}

BooleanSetting::~BooleanSetting() = default;

bool BooleanSetting::get() const
{
  return super::get() == BooleanSettings::BOOLEAN_SETTING_TRUE;
}

void BooleanSetting::toggle()
{
  set(get() ? BooleanSettings::BOOLEAN_SETTING_FALSE : BooleanSettings::BOOLEAN_SETTING_TRUE);
}

const std::vector<Glib::ustring> &BooleanSetting::enumToString() const
{
  static std::vector<Glib::ustring> s_modeNames = { "on", "off" };
  return s_modeNames;
}

const std::vector<Glib::ustring> &BooleanSetting::enumToDisplayString() const
{
  static std::vector<Glib::ustring> s_modeNames = { "On", "Off" };
  return s_modeNames;
}

void BooleanSetting::incDec(int inc, bool wrap)
{
  if((wrap && inc % 2 != 0) || ((inc > 0 && get()) || (inc < 0 && !get())))
    toggle();
}

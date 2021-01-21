#include "BooleanSetting.h"

BooleanSetting::BooleanSetting(UpdateDocumentContributor &settings, bool def)
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
  //TODO schauen wies sich verhaelt
  if(wrap)
  {
    for(auto i = 0; i < inc; i++)
      toggle();
  }
  else if((inc < 0 && get()) || (inc > 0 && !get()))
  {
    toggle();
  }
}

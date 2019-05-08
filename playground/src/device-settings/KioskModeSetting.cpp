#include <device-settings/KioskModeSetting.h>
#include <xml/FileOutStream.h>
#include <device-settings/DebugLevel.h>

KioskModeSetting::KioskModeSetting(Settings& settings)
    : BooleanSetting(settings, FileOutStream::getKioskMode())
{
}

KioskModeSetting::~KioskModeSetting()
{
}

bool KioskModeSetting::setBool(bool value)
{
  if(FileOutStream::getKioskMode() != value)
  {
    FileOutStream::setKioskMode(value);
    BooleanSetting::set(value ? BooleanSetting::tEnum::BOOLEAN_SETTING_TRUE
                              : BooleanSetting::tEnum::BOOLEAN_SETTING_FALSE);
  }
  return value;
}

bool KioskModeSetting::set(Glib::ustring value)
{
  return setBool(value.find("on") != Glib::ustring::npos);
}

void KioskModeSetting::setSetting(Initiator initiator, const Glib::ustring& text)
{
  set(text);
  BooleanSetting::setSetting(initiator, text);
}

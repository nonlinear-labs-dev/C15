#include "TimeTools.h"

#include <ctime>
#include <iomanip>
#include <memory>
#include <sstream>

#include <device-settings/DateTimeAdjustment.h>
#include <device-settings/DebugLevel.h>
#include <device-settings/Settings.h>
#include <Application.h>

time_t TimeTools::getRealTimestamp()
{
  return time(nullptr);
}

time_t TimeTools::getAdjustedTimestamp()
{
  if(Application::exists())
  {
    auto dateTimeSetting = Application::get().getSettings()->getSetting<DateTimeAdjustment>();
    return getRealTimestamp() + dateTimeSetting->get();
  }

  return getRealTimestamp();
}

Glib::ustring TimeTools::getRealIso()
{
  return getIsoTime(getRealTimestamp());
}

Glib::ustring TimeTools::getAdjustedIso()
{
  return getIsoTime(getAdjustedTimestamp());
}

Glib::ustring TimeTools::getIsoTime(const time_t stamp)
{
  char buf[128];
  strftime(buf, sizeof buf, "%FT%TZ", gmtime(&stamp));
  return buf;
}

Glib::ustring TimeTools::getLocalTime(const time_t stamp)
{
  char buf[128];
  strftime(buf, sizeof buf, "%FT%TZ", localtime(&stamp));
  return buf;
}

Glib::ustring TimeTools::getDisplayStringFromIso(const Glib::ustring &iso)
{
  return formatTime(iso, "%F %R");
}

Glib::ustring TimeTools::getDisplayStringFromStamp(time_t stamp)
{
  return formatTime(stamp, "%F %R");
}

Glib::ustring TimeTools::formatTime(time_t secondsSinceUnixEpoch, const Glib::ustring &format)
{
  auto tm = gmtime(&secondsSinceUnixEpoch);
  return formatTime(tm, format);
}

Glib::ustring TimeTools::formatTime(const Glib::ustring &iso, const Glib::ustring &format)
{
  struct tm t;

  if(!strptime(iso.raw().c_str(), "%Y-%m-%dT%H:%M:%S%z", &t))
  {
    DebugLevel::warning("could not parse time value", iso);
    return "---";
  }

  return formatTime(&t, format);
}

Glib::ustring TimeTools::formatTime(const tm *tm, const Glib::ustring &format)
{
  std::stringstream ss;
  ss << std::put_time(tm, format.c_str());
  return ss.str();
}

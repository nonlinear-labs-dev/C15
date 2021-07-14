#pragma once

#include <glibmm/ustring.h>
#include <chrono>

class TimeTools
{
 public:
  static Glib::ustring getAdjustedIso();
  static time_t getAdjustedTimestamp();

  static Glib::ustring getDisplayStringFromIso(const Glib::ustring &iso);
  static Glib::ustring getDisplayStringFromStamp(time_t stamp);
  static Glib::ustring getIsoTime(time_t stamp);
  static Glib::ustring getLocalTime(time_t stamp);

 private:
  static Glib::ustring formatTime(time_t secondsSinceUnixEpoch, const Glib::ustring &format);
  static Glib::ustring formatTime(const Glib::ustring &iso, const Glib::ustring &format);
  static Glib::ustring formatTime(const tm *tm, const Glib::ustring &format);

  static time_t getRealTimestamp();
  static Glib::ustring getRealIso();
};

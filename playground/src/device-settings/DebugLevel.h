#pragma once

#include "EnumSetting.h"
#include <sys/time.h>

enum class DebugLevels
{
  DEBUG_LEVEL_DEBUG,
  DEBUG_LEVEL_SILENT,
  DEBUG_LEVEL_ERROR,
  DEBUG_LEVEL_WARNING,
  DEBUG_LEVEL_INFO,
  DEBUG_LEVEL_GASSY
};

inline std::ostream& operator<<(std::ostream& out, const DebugLevels& level)
{
  switch(level)
  {
    case DebugLevels::DEBUG_LEVEL_SILENT:
      out << "silent";
      break;

    case DebugLevels::DEBUG_LEVEL_ERROR:
      out << "!!! ERROR !!!";
      break;

    case DebugLevels::DEBUG_LEVEL_WARNING:
      out << "WARNING";
      break;

    case DebugLevels::DEBUG_LEVEL_DEBUG:
      out << "Debug";
      break;

    case DebugLevels::DEBUG_LEVEL_INFO:
      out << "info";
      break;

    case DebugLevels::DEBUG_LEVEL_GASSY:
      out << "gassy";
      break;
  }

  return out;
}

class DebugLevel : public EnumSetting<DebugLevels>
{
 private:
  typedef EnumSetting<DebugLevels> super;

 public:
  DebugLevel(Settings& settings);
  virtual ~DebugLevel();

  bool set(DebugLevels m) override;

  static DebugLevels getLevel();

  template <typename... tArgs> static void gassy(const tArgs&... args)
  {
    trace(DebugLevels::DEBUG_LEVEL_GASSY, args...);
  }

  template <typename... tArgs> static void info(const tArgs&... args)
  {
    trace(DebugLevels::DEBUG_LEVEL_INFO, args...);
  }

  template <typename... tArgs> static void debug(const tArgs&... args)
  {
    trace(DebugLevels::DEBUG_LEVEL_DEBUG, args...);
  }

  template <typename... tArgs> static void warning(const tArgs&... args)
  {
    trace(DebugLevels::DEBUG_LEVEL_WARNING, args...);
  }

  template <typename... tArgs> static void error(const tArgs&... args)
  {
    trace(DebugLevels::DEBUG_LEVEL_ERROR, args...);
  }

  template <typename... tArgs> static void trace(DebugLevels level, const tArgs&... args)
  {
    if(DebugLevel::getLevel() >= level)
      printTrace(level, args...);
  }

  template <typename... tArgs> static void output(const tArgs&... args)
  {
    std::stringstream str;
    printTrace(str, args...);
    g_printerr("%s\n", str.str().c_str());
  }

  template <typename... tArgs> static void throwException(const tArgs&... args)
  {
    auto str = concat(args...);
    throw std::runtime_error(str);
  }

 private:
  DebugLevel(const DebugLevel& other);
  DebugLevel& operator=(const DebugLevel&);

  const std::vector<Glib::ustring>& enumToString() const override;
  const std::vector<Glib::ustring>& enumToDisplayString() const override;

  template <typename... tArgs> static void printTrace(DebugLevels level, tArgs&... args)
  {
    std::stringstream str;
    str << level << " ";
    printTrace(str, args...);
  }

  template <typename tFirst, typename... tArgs>
  static void printTrace(std::stringstream& str, const tFirst& first, const tArgs&... args)
  {
    str << first << " ";
    printTrace(str, args...);
  }

  template <typename tFirst> static void printTrace(std::stringstream& str, const tFirst& first)
  {
    str << first;
    g_printerr("%8" G_GUINT64_FORMAT ": %s\n", getTimestamp(), str.str().c_str());
  }

  static uint64_t getTimestamp()
  {
    static uint64_t epoch = getCurrentMilliseconds();
    return getCurrentMilliseconds() - epoch;
  }

  static uint64_t getCurrentMilliseconds()
  {
    struct timeval spec;
    gettimeofday(&spec, nullptr);
    return spec.tv_sec * 1000 + spec.tv_usec / 1000;
  }
};

#define DEBUG_TRACE(...) DebugLevel::debug(__PRETTY_FUNCTION__, "Line", __LINE__, "->", __VA_ARGS__)
#define DEBUG_TRACE_EMPTY() DebugLevel::debug(__PRETTY_FUNCTION__, "Line", __LINE__)

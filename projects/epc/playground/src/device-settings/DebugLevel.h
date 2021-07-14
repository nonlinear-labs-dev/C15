#pragma once

#include "EnumSetting.h"
#include <nltools/logging/Log.h>

enum class DebugLevels
{
  DEBUG_LEVEL_SILENT,
  DEBUG_LEVEL_ERROR,
  DEBUG_LEVEL_WARNING,
  DEBUG_LEVEL_INFO,
  DEBUG_LEVEL_DEBUG,
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

class Settings;

class DebugLevel : public EnumSetting<DebugLevels>
{
 private:
  typedef EnumSetting<DebugLevels> super;

 public:
  explicit DebugLevel(Settings& settings);
  ~DebugLevel() override;

  bool set(DebugLevels m) override;

  template <typename... tArgs> static void gassy(const tArgs&... args)
  {
    nltools::Log::debug(args...);
  }

  template <typename... tArgs> static void debug(const tArgs&... args)
  {
    nltools::Log::debug(args...);
  }

  template <typename... tArgs> static void info(const tArgs&... args)
  {
    nltools::Log::info(args...);
  }

  template <typename... tArgs> static void warning(const tArgs&... args)
  {
    nltools::Log::warning(args...);
  }

  template <typename... tArgs> static void error(const tArgs&... args)
  {
    nltools::Log::error(args...);
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

  template <typename... tArgs> static std::string concat(tArgs&... args)
  {
    std::stringstream str;
    (void) std::initializer_list<bool> { (str << args << " ", false)... };
    return str.str();
  }

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
    printError(str.str());
  }

  static void printError(const std::string& str);
  static uint64_t getTimestamp();
  static uint64_t getCurrentMilliseconds();
};

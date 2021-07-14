#pragma once

#include <nltools/GlibFwd.h>
#include <stdlib.h>
#include <type_traits>
#include <string>
#include <memory>
#include "playground-helpers.h"
#include <glibmm/ustring.h>

void printLastFunctions();

typedef int32_t tAudioControlValue;

namespace std
{
  template <> struct hash<Glib::ustring> : public __hash_base<size_t, Glib::ustring>
  {
    size_t operator()(const Glib::ustring& p) const noexcept
    {
      std::hash<std::string> h;
      return h(p.raw());
    }
  };

  inline basic_istream<char>& getline(basic_istream<char>& in, Glib::ustring& target)
  {
    std::string str;
    auto& res = getline(in, str);
    target = str;
    return res;
  }

  template <typename T, std::enable_if_t<std::is_enum<T>::value, bool> = true> std::string to_string(const T& e)
  {
    return to_string(static_cast<int>(e));
  }

  inline Glib::ustring to_string(const Glib::ustring& in)
  {
    return in;
  }
}

template <typename T, std::enable_if_t<std::is_enum<T>::value, bool> = true>
std::ostream& operator<<(std::ostream& stream, const T& e)
{
  wirteIntToStream(stream, static_cast<int>(e));
  return stream;
}

template <class T> inline void hash_combine(std::size_t& seed, const T& v)
{
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <typename T> inline std::string to_string(const T& t)
{
  return std::to_string(t);
}

template <> inline std::string to_string<std::string>(const std::string& t)
{
  return t;
}

typedef int16_t tTcdValue;
typedef double tDisplayValue;
typedef double tControlPositionValue;

enum class Initiator
{
  EXPLICIT_LOAD,
  EXPLICIT_WEBUI,
  EXPLICIT_HWUI,
  EXPLICIT_PLAYCONTROLLER,
  EXPLICIT_OTHER,
  EXPLICIT_MCVIEW,
  EXPLICIT_USECASE,
  INDIRECT_SPLIT_SYNC,
  INDIRECT
};

enum class PresetType : uint8_t
{
  Single,
  Morph,
  Multi
};

enum class SaveResult : uint8_t
{
  Nothing,
  Again,
  Finished
};

std::string getStackTrace(const std::string& prefix);

#ifdef _TESTS
#define FOR_TESTS(expression) expression
#else
#define FOR_TESTS(expression)
#endif

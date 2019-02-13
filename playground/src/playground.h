#pragma once

#include <glib.h>
#include <glibmm.h>
#include <giomm.h>
#include <sigc++/sigc++.h>
#include <sigc++/trackable.h>
#include <memory>
#include <stdio.h>
#include <iostream>

#define TRACE_LOC_AND_ARGS(loc, arg) std::cout << loc << ": " << arg << std::endl
#define TRACE(arg) TRACE_LOC_AND_ARGS(G_STRLOC, arg)
#define TRACE_CALL() TRACE_LOC_AND_ARGS(G_STRLOC, "called")

#include "tools/Signal.h"

//void __cyg_profile_func_enter (void *this_fn, void *call_site) __attribute__((no_instrument_function));
//void __cyg_profile_func_exit  (void *this_fn, void *call_site) __attribute__((no_instrument_function));
void printLastFunctions();

#include "Compatibility.h"

using namespace Glib;
using namespace std;
using namespace sigc;

typedef gint32 tAudioControlValue;

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
}
template <typename T, std::enable_if_t<std::is_enum<T>::value, bool> = true> std::string to_string(const T& e)
{
  return to_string(static_cast<int>(e));
}

template <typename T, std::enable_if_t<std::is_enum<T>::value, bool> = true>
std::ostream& operator<<(std::ostream& stream, const T& e)
{
  stream << static_cast<int>(e);
  return stream;
}

template <class T> inline void hash_combine(std::size_t& seed, const T& v)
{
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

typedef gint16 tTcdValue;
typedef double tDisplayValue;
typedef double tControlPositionValue;

enum class Initiator
{
  EXPLICIT_WEBUI,
  EXPLICIT_HWUI,
  EXPLICIT_LPC,
  EXPLICIT_OTHER,
  EXPLICIT_MCVIEW,
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

enum class ModulationSource
{
  NONE = 0,
  MC1 = 1,
  MC2 = 2,
  MC3 = 3,
  MC4 = 4,
  NUM_CHOICES = 5
};

enum class RibbonTouchBehaviour
{
  ABSOLUTE,
  RELATIVE,
  NUM_TOUCH_BEHAVIOURS
};

enum class RibbonReturnMode
{
  STAY = 0,
  RETURN = 1,
  NUM_RETURN_MODES
};

enum class PedalModes
{
  STAY = 0,
  RETURN_TO_ZERO = 1,
  RETURN_TO_CENTER = 2,
  NUM_PEDAL_MODES
};

enum class ReturnMode
{
  None,
  Center,
  Zero
};

#ifdef _TESTS
#define FOR_TESTS(expression) expression
#else
#define FOR_TESTS(expression)
#endif

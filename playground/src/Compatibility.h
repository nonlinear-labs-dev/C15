#pragma once

#include <glib.h>

#if((__cplusplus >= 201103L) && defined(_GLIBCXX_USE_C99) && !defined(_GLIBCXX_HAVE_BROKEN_VSWPRINTF))
#else

#include <stdlib.h>
#include <stdio.h>

namespace std
{
  inline int stoi(const Glib::ustring& str)
  {
    return atoi(str.c_str());
  }

  inline long stol(const Glib::ustring& str)
  {
    return strtol(str.c_str(), NULL, 10);
  }

  inline unsigned long stoul(const Glib::ustring& str)
  {
    return strtoul(str.c_str(), NULL, 10);
  }

  inline long long stoll(const Glib::ustring& str)
  {
    return strtoll(str.c_str(), NULL, 10);
  }

  inline unsigned long long stoull(const Glib::ustring& str)
  {
    return strtoull(str.c_str(), NULL, 10);
  }

  // NB: strtof vs strtod.
  inline float stof(const Glib::ustring& str)
  {
    return strtof(str.c_str(), NULL);
  }

  inline double stod(const Glib::ustring& str)
  {
    return strtof(str.c_str(), NULL);
  }

  inline long double stold(const Glib::ustring& str, size_t* idx = 0)
  {
    return strtold(str.c_str(), NULL);
  }

  template <typename T> Glib::ustring to_string(T val, const char* format)
  {
    char txt[256];
    sprintf(txt, format, val);
    return txt;
  }

  inline Glib::ustring to_string(int val)
  {
    return to_string(val, "%d");
  }

  inline Glib::ustring to_string(unsigned val)
  {
    return to_string(val, "%u");
  }

  inline Glib::ustring to_string(long val)
  {
    return to_string(val, "%ld");
  }

  inline Glib::ustring to_string(unsigned long val)
  {
    return to_string(val, "%lu");
  }

  inline Glib::ustring to_string(long long val)
  {
    return to_string(val, "%lld");
  }

  inline Glib::ustring to_string(unsigned long long val)
  {
    return to_string(val, "%llu");
  }

  inline Glib::ustring to_string(float val)
  {
    return to_string(val, "%f");
  }

  inline Glib::ustring to_string(double val)
  {
    return to_string(val, "%f");
  }

  inline Glib::ustring to_string(long double val)
  {
    return to_string(val, "%Lf");
  }
}

#endif

namespace std
{
  inline Glib::ustring to_string(const Glib::ustring &in)
  {
    return in;
  }
}

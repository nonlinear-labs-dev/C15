#pragma once

#include "logging/Log.h"
#include <stdexcept>

namespace nltools
{
  template <class T> void ignore(const T &)
  {
  }

  inline void fail[[noreturn]](const char *expr, const char *file, int line, const char *function)
  {
    Log::error("Assertion failed:");
    Log::output<Log::LogMode::AppendNewLine>("\tExpression: ", expr);
    Log::output<Log::LogMode::AppendNewLine>("\tLocation: ", file, ":", line);
    Log::output<Log::LogMode::AppendNewLine>("\tFunction: ", function);
    throw std::runtime_error("Assertion failed!");
  }
}

#define nltools_assertAlways(expr)                                                                                     \
  (static_cast<bool>(expr) ? void(0) : nltools::fail(#expr, __FILE__, __LINE__, __PRETTY_FUNCTION__))

#if _DEVELOPMENT_PC
#define nltools_assertOnDevPC(a) nltools_assertAlways(a)
#else
#define nltools_assertOnDevPC(a) ((void) (0))
#endif

#ifndef NDEBUG
#define nltools_assertInDebug(a) nltools_assertAlways(a)
#else
#define nltools_assertInDebug(a) ((void) (0))
#endif

#ifdef _TESTS
#define nltools_assertInTest(a) nltools_assertAlways(a)
#else
#define nltools_assertInTest(a) ((void) (0))
#endif

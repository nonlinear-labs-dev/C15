#pragma once

#include <initializer_list>
#include <iostream>

class Log
{
 public:
  enum Level
  {
    Debug,
    Info,
    Warning,
    Error
  };

  enum class LogMode
  {
    Plain,
    AppendNewLine,
    InsertSpaces,
    InsertSpacesAndAppendNewLine
  };

  template <LogMode mode = LogMode::InsertSpacesAndAppendNewLine, typename... Args>
  static void debug(const Args&... args)
  {
    if(s_level <= Level::Debug)
      output<mode>("Debug: ", args...);
  }

  template <LogMode mode = LogMode::InsertSpacesAndAppendNewLine, typename... Args>
  static void info(const Args&... args)
  {
    if(s_level <= Level::Info)
      output<mode>("Info: ", args...);
  }

  template <LogMode mode = LogMode::InsertSpacesAndAppendNewLine, typename... Args>
  static void warning(const Args&... args)
  {
    if(s_level <= Level::Warning)
      output<mode>("Warning: ", args...);
  }

  template <LogMode mode = LogMode::InsertSpacesAndAppendNewLine, typename... Args>
  static void error(const Args&... args)
  {
    if(s_level <= Level::Error)
      output<mode>("Error: ", args...);
  }

  static void flush()
  {
    std::cout << std::flush;
  }

  template <LogMode mode = LogMode::InsertSpacesAndAppendNewLine, typename... Args>
  static void output(const Args&... args)
  {
    constexpr auto addNewLine = mode == LogMode::AppendNewLine || mode == LogMode::InsertSpacesAndAppendNewLine;
    constexpr auto insertSpaces = mode == LogMode::InsertSpaces || mode == LogMode::InsertSpacesAndAppendNewLine;

    if(insertSpaces)
      std::initializer_list<bool>({ (std::cout << args, std::cout << " ", false)... });
    else
      std::initializer_list<bool>({ (std::cout << args, false)... });

    if(addNewLine)
      std::cout << std::endl;
  }

 private:
  static Level s_level;
};

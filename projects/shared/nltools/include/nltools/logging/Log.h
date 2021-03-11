#pragma once

#include "../StringTools.h"

namespace nltools
{
  class Log
  {
   public:
    enum Level
    {
      Debug,
      Info,
      Warning,
      Error,
      Notify,
      Silent
    };

    static void setLevel(Level l)
    {
      s_level = l;
    }

    static Level getLevel()
    {
      return s_level;
    }

    enum class LogMode
    {
      Plain,
      AppendNewLine,
      InsertSpaces,
      InsertSpacesAndAppendNewLine
    };

    template <LogMode mode = LogMode::InsertSpacesAndAppendNewLine, typename... Args>
    static void debug(const Args &... args)
    {
      if(s_level <= Level::Debug)
        output<mode>("Debug: ", args...);
    }

    template <LogMode mode = LogMode::InsertSpacesAndAppendNewLine, typename... Args>
    static void info(const Args &... args)
    {
      if(s_level <= Level::Info)
        output<mode>("Info: ", args...);
    }

    template <LogMode mode = LogMode::InsertSpacesAndAppendNewLine, typename... Args>
    static void warning(const Args &... args)
    {
      if(s_level <= Level::Warning)
        output<mode>("Warning: ", args...);
    }

    template <LogMode mode = LogMode::InsertSpacesAndAppendNewLine, typename... Args>
    static void error(const Args &... args)
    {
      if(s_level <= Level::Error)
        output<mode>("Error: ", args...);
    }

    template <LogMode mode = LogMode::InsertSpacesAndAppendNewLine, typename... Args>
    static void notify(const Args &... args)
    {
      if(s_level <= Level::Notify)
        output<mode>("Notify: ", args...);
    }

    static void flush();
    static void print(const std::string &str);
    static void printNewLine();

    template <LogMode mode = LogMode::InsertSpacesAndAppendNewLine, typename... Args>
    static void output(const Args &... args)
    {
      constexpr auto addNewLine = mode == LogMode::AppendNewLine || mode == LogMode::InsertSpacesAndAppendNewLine;
      constexpr auto insertSpaces = mode == LogMode::InsertSpaces || mode == LogMode::InsertSpacesAndAppendNewLine;

      if(insertSpaces)
        print(nltools::string::concatWithDelimiter(' ', args...));
      else
        print(nltools::string::concat(args...));

      if(addNewLine)
        printNewLine();
    }

    template <typename... tArgs> static void throwException(const tArgs &... args)
    {
      auto str = nltools::string::concat(args...);
      error(str);
      throw std::runtime_error(str);
    }

   private:
    static Level s_level;
  };
}

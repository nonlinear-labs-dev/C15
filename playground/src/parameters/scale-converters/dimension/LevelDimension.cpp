#include "LevelDimension.h"

LevelDimension::LevelDimension()
{
}

LevelDimension::~LevelDimension()
{
}

const LevelDimension &LevelDimension::get()
{
  static LevelDimension d;
  return d;
}

Glib::ustring LevelDimension::getStingizerJS() const
{
  return "((cpValue == -128.0) ? (withUnit ? '-inf dB' : '-inf') : formatDimensionRounded(cpValue, 'dB', withUnit, "
         "0.1))";
}

Glib::ustring LevelDimension::stringize(const tDisplayValue &displayValue) const
{
  if(displayValue == silence())
    return "-inf dB";

  return formatDimensionRounded(displayValue, "dB", 0.1);
}

#include "LevelDimension3Digits.h"

LevelDimension3Digits::LevelDimension3Digits()
{
}

LevelDimension3Digits::~LevelDimension3Digits()
{
}

const LevelDimension3Digits &LevelDimension3Digits::get()
{
  static LevelDimension3Digits d;
  return d;
}

Glib::ustring LevelDimension3Digits::getStingizerJS() const
{
  return "((cpValue == -128.0) ? (withUnit ? '-inf dB' : '-inf') : formatDimensionDigits(cpValue, 'dB', withUnit, 3))";
}

Glib::ustring LevelDimension3Digits::stringize(const tDisplayValue &displayValue) const
{
  if(displayValue == silence())
    return "-inf dB";

  return formatDimensionDigits(displayValue, "dB", 3);
}

#include "Dimension.h"
#include <cmath>

Dimension::Dimension()
{
}

Glib::ustring Dimension::formatDimensionRounded(tControlPositionValue in, const Glib::ustring &unit,
                                                tControlPositionValue roundTo)
{
  int cnt = 0;

  while(round(roundTo) < 1)
  {
    roundTo *= 10;
    in *= 10;
    cnt++;
  }

  roundTo = round(roundTo);

  in /= roundTo;
  in = round(in);

  if(in == -0.0)
    in = 0;

  in *= roundTo;

  in *= pow(10, -cnt);
  char format[128];
  sprintf(format, "%%.%df", cnt);

  char txt[128];
  sprintf(txt, format, in);
  return Glib::ustring(txt) + " " + unit;
}

bool willTruncateToZero(tControlPositionValue in, int numDigits)
{
  return fabs(in) < pow(10, -numDigits);
}

Glib::ustring Dimension::formatDimensionDigits(tControlPositionValue in, const Glib::ustring &unit, int numDigits)
{
  if(in == std::numeric_limits<tControlPositionValue>::infinity())
    return "inf";

  if(willTruncateToZero(in, numDigits))
    return formatPositiveDimensionDigits(0, unit, numDigits);
  else if(in < 0)
    return formatNegativeDimensionDigits(in, unit, numDigits);
  else
    return formatPositiveDimensionDigits(in, unit, numDigits);
}

Glib::ustring Dimension::formatNegativeDimensionDigits(tControlPositionValue in, const Glib::ustring &unit,
                                                       int numDigits)
{
  Glib::ustring minus = "-";
  return minus + formatPositiveDimensionDigits(-in, unit, numDigits);
}

bool tooSmall(tControlPositionValue in, int lowerBorder)
{
  return round(in) < lowerBorder;
}

bool tooBig(tControlPositionValue in, int upperBorder)
{
  return round(in) >= upperBorder;
}

Glib::ustring Dimension::formatPositiveDimensionDigits(tControlPositionValue in, const Glib::ustring &unit,
                                                       int numDigits)
{
  int lowerBorder = pow(10, numDigits - 1);
  int upperBorder = lowerBorder * 10;

  if(tooSmall(in, lowerBorder))
  {
    int cnt = 0;

    while(tooSmall(in, lowerBorder) && in != 0.0)
    {
      in *= 10;
      cnt++;
    }

    if(in == 0.0)
      cnt = 3;

    in = round(in);
    in *= pow(10, -cnt);

    int postComma = std::min(3, cnt);
    char format[256];
    sprintf(format, "%%.%df", postComma);

    char txt[256];
    sprintf(txt, format, in);
    return Glib::ustring(txt) + " " + unit;
  }
  else if(tooBig(in, upperBorder))
  {
    int cnt = 0;

    while(tooBig(in, upperBorder) && in != 0.0)
    {
      in /= 10;
      cnt++;
    }

    in = round(in);
    in *= pow(10, cnt);

    char txt[256];
    sprintf(txt, "%3.0f", in);
    return Glib::ustring(txt) + " " + unit;
  }

  else
  {
    char txt[256];
    sprintf(txt, "%3.0f", in);
    return Glib::ustring(txt) + " " + unit;
  }
}

Glib::ustring Dimension::getStingizerJS() const
{
  return "";
}

size_t Dimension::hash() const
{
  auto &foo = typeid(*this);
  return foo.hash_code();
}
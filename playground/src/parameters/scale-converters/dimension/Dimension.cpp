#include "Dimension.h"
#include "testing/TestDriver.h"
#include <cmath>
#include <math.h>

static TestDriver<Dimension> tests;

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

void Dimension::registerTests()
{
  class TestDimension : public Dimension
  {
   public:
    Glib::ustring stringize(const tDisplayValue &displayValue) const
    {
      return "";
    }
  };

  g_test_add_func("/Dimension/formatDimensionDigits", []() {
    TestDimension d;

    g_assert(d.formatDimensionDigits(0, "foo", 3) == "0.000 foo");

    g_assert(d.formatDimensionDigits(0.000001, "foo", 3) == "0.000 foo");
    g_assert(d.formatDimensionDigits(0.00001, "foo", 3) == "0.000 foo");
    g_assert(d.formatDimensionDigits(0.0001, "foo", 3) == "0.000 foo");
    g_assert(d.formatDimensionDigits(0.001, "foo", 3) == "0.001 foo");
    g_assert(d.formatDimensionDigits(0.01, "foo", 3) == "0.010 foo");
    g_assert(d.formatDimensionDigits(0.1, "foo", 3) == "0.100 foo");
    g_assert(d.formatDimensionDigits(1.000001, "foo", 3) == "1.00 foo");
    g_assert(d.formatDimensionDigits(1.200001, "foo", 3) == "1.20 foo");
    g_assert(d.formatDimensionDigits(1.230001, "foo", 3) == "1.23 foo");
    g_assert(d.formatDimensionDigits(1.234001, "foo", 3) == "1.23 foo");
    g_assert(d.formatDimensionDigits(10.000001, "foo", 3) == "10.0 foo");
    g_assert(d.formatDimensionDigits(123.000001, "foo", 3) == "123 foo");
    g_assert(d.formatDimensionDigits(1234, "foo", 3) == "1230 foo");
    g_assert(d.formatDimensionDigits(1234567, "foo", 3) == "1230000 foo");

    g_assert(d.formatDimensionDigits(-0.000001, "foo", 3) == "0.000 foo");
    g_assert(d.formatDimensionDigits(-0.00001, "foo", 3) == "0.000 foo");
    g_assert(d.formatDimensionDigits(-0.0001, "foo", 3) == "0.000 foo");
    g_assert(d.formatDimensionDigits(-0.001, "foo", 3) == "-0.001 foo");
    g_assert(d.formatDimensionDigits(-0.01, "foo", 3) == "-0.010 foo");
    g_assert(d.formatDimensionDigits(-0.1, "foo", 3) == "-0.100 foo");
    g_assert(d.formatDimensionDigits(-1.000001, "foo", 3) == "-1.00 foo");
    g_assert(d.formatDimensionDigits(-1.200001, "foo", 3) == "-1.20 foo");
    g_assert(d.formatDimensionDigits(-1.230001, "foo", 3) == "-1.23 foo");
    g_assert(d.formatDimensionDigits(-1.234001, "foo", 3) == "-1.23 foo");
    g_assert(d.formatDimensionDigits(-10.000001, "foo", 3) == "-10.0 foo");
    g_assert(d.formatDimensionDigits(-123.000001, "foo", 3) == "-123 foo");
    g_assert(d.formatDimensionDigits(-1234, "foo", 3) == "-1230 foo");
    g_assert(d.formatDimensionDigits(-1234567, "foo", 3) == "-1230000 foo");
  });

  g_test_add_func("/Dimension/formatDimensionRounded", []() {
    TestDimension d;

    g_assert(d.formatDimensionRounded(0, "foo", 1) == "0 foo");
    g_assert(d.formatDimensionRounded(0, "foo", 0.1) == "0.0 foo");
    g_assert(d.formatDimensionRounded(0, "foo", 0.01) == "0.00 foo");
    g_assert(d.formatDimensionRounded(1.25, "foo", 0.02) == "1.26 foo");

    g_assert(d.formatDimensionRounded(0.000001, "foo", 0.1) == "0.0 foo");
    g_assert(d.formatDimensionRounded(0.00001, "foo", 0.1) == "0.0 foo");
    g_assert(d.formatDimensionRounded(0.0001, "foo", 0.1) == "0.0 foo");
    g_assert(d.formatDimensionRounded(0.001, "foo", 0.1) == "0.0 foo");
    g_assert(d.formatDimensionRounded(0.01, "foo", 0.1) == "0.0 foo");
    g_assert(d.formatDimensionRounded(0.1, "foo", 0.1) == "0.1 foo");
    g_assert(d.formatDimensionRounded(1.000001, "foo", 0.1) == "1.0 foo");
    g_assert(d.formatDimensionRounded(1.200001, "foo", 0.1) == "1.2 foo");
    g_assert(d.formatDimensionRounded(1.230001, "foo", 0.1) == "1.2 foo");
    g_assert(d.formatDimensionRounded(1.246001, "foo", 0.1) == "1.2 foo");
    g_assert(d.formatDimensionRounded(10.000001, "foo", 0.1) == "10.0 foo");
    g_assert(d.formatDimensionRounded(123.000001, "foo", 0.1) == "123.0 foo");
    g_assert(d.formatDimensionRounded(1234, "foo", 0.1) == "1234.0 foo");
    g_assert(d.formatDimensionRounded(1234567, "foo", 0.1) == "1234567.0 foo");

    g_assert(d.formatDimensionRounded(-0.000001, "foo", 0.1) == "0.0 foo");
    g_assert(d.formatDimensionRounded(-0.00001, "foo", 0.1) == "0.0 foo");
    g_assert(d.formatDimensionRounded(-0.0001, "foo", 0.1) == "0.0 foo");
    g_assert(d.formatDimensionRounded(-0.001, "foo", 0.1) == "0.0 foo");
    g_assert(d.formatDimensionRounded(-0.01, "foo", 0.1) == "0.0 foo");
    g_assert(d.formatDimensionRounded(-0.1, "foo", 0.1) == "-0.1 foo");
    g_assert(d.formatDimensionRounded(-1.000001, "foo", 0.1) == "-1.0 foo");
    g_assert(d.formatDimensionRounded(-1.200001, "foo", 0.1) == "-1.2 foo");
    g_assert(d.formatDimensionRounded(-1.230001, "foo", 0.1) == "-1.2 foo");
    g_assert(d.formatDimensionRounded(-1.246001, "foo", 0.1) == "-1.2 foo");
    g_assert(d.formatDimensionRounded(-10.000001, "foo", 0.1) == "-10.0 foo");
    g_assert(d.formatDimensionRounded(-123.000001, "foo", 0.1) == "-123.0 foo");
    g_assert(d.formatDimensionRounded(-1234, "foo", 0.1) == "-1234.0 foo");
    g_assert(d.formatDimensionRounded(-1234567, "foo", 0.1) == "-1234567.0 foo");
  });
}

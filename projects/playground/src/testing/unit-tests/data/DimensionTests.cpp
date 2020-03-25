#include <testing/TestHelper.h>
#include <parameters/scale-converters/dimension/Dimension.h>

class TestableDimension : public Dimension
{
 public:
  Glib::ustring stringize(const tDisplayValue& displayValue) const
  {
    return "";
  }

  Glib::ustring formatDimensionDigits(tControlPositionValue val, const Glib::ustring& in, int n)
  {
    return Dimension::formatDimensionDigits(val, in, n);
  }

  Glib::ustring formatDimensionRounded(tControlPositionValue val, const Glib::ustring& in,
                                       tControlPositionValue roundTo)
  {
    return Dimension::formatDimensionRounded(val, in, roundTo);
  }
};

using namespace std::string_literals;

TEST_CASE("Format Dimension Digits", "[Dimension]")
{
  TestableDimension d;

  CHECK(d.formatDimensionDigits(0, "foo", 3) == "0.000 foo"s);

  CHECK(d.formatDimensionDigits(0.000001, "foo", 3) == "0.000 foo"s);
  CHECK(d.formatDimensionDigits(0.00001, "foo", 3) == "0.000 foo"s);
  CHECK(d.formatDimensionDigits(0.0001, "foo", 3) == "0.000 foo"s);
  CHECK(d.formatDimensionDigits(0.001, "foo", 3) == "0.001 foo"s);
  CHECK(d.formatDimensionDigits(0.01, "foo", 3) == "0.010 foo"s);
  CHECK(d.formatDimensionDigits(0.1, "foo", 3) == "0.100 foo"s);
  CHECK(d.formatDimensionDigits(1.000001, "foo", 3) == "1.00 foo"s);
  CHECK(d.formatDimensionDigits(1.200001, "foo", 3) == "1.20 foo"s);
  CHECK(d.formatDimensionDigits(1.230001, "foo", 3) == "1.23 foo"s);
  CHECK(d.formatDimensionDigits(1.234001, "foo", 3) == "1.23 foo"s);
  CHECK(d.formatDimensionDigits(10.000001, "foo", 3) == "10.0 foo"s);
  CHECK(d.formatDimensionDigits(123.000001, "foo", 3) == "123 foo"s);
  CHECK(d.formatDimensionDigits(1234, "foo", 3) == "1230 foo"s);
  CHECK(d.formatDimensionDigits(1234567, "foo", 3) == "1230000 foo"s);

  CHECK(d.formatDimensionDigits(-0.000001, "foo", 3) == "0.000 foo"s);
  CHECK(d.formatDimensionDigits(-0.00001, "foo", 3) == "0.000 foo"s);
  CHECK(d.formatDimensionDigits(-0.0001, "foo", 3) == "0.000 foo"s);
  CHECK(d.formatDimensionDigits(-0.001, "foo", 3) == "-0.001 foo"s);
  CHECK(d.formatDimensionDigits(-0.01, "foo", 3) == "-0.010 foo"s);
  CHECK(d.formatDimensionDigits(-0.1, "foo", 3) == "-0.100 foo"s);
  CHECK(d.formatDimensionDigits(-1.000001, "foo", 3) == "-1.00 foo"s);
  CHECK(d.formatDimensionDigits(-1.200001, "foo", 3) == "-1.20 foo"s);
  CHECK(d.formatDimensionDigits(-1.230001, "foo", 3) == "-1.23 foo"s);
  CHECK(d.formatDimensionDigits(-1.234001, "foo", 3) == "-1.23 foo"s);
  CHECK(d.formatDimensionDigits(-10.000001, "foo", 3) == "-10.0 foo"s);
  CHECK(d.formatDimensionDigits(-123.000001, "foo", 3) == "-123 foo"s);
  CHECK(d.formatDimensionDigits(-1234, "foo", 3) == "-1230 foo"s);
  CHECK(d.formatDimensionDigits(-1234567, "foo", 3) == "-1230000 foo"s);
}

TEST_CASE("formatDimensionRounded", "[Dimension]")
{
  TestableDimension d;

  CHECK(d.formatDimensionRounded(0, "foo", 1) == "0 foo"s);
  CHECK(d.formatDimensionRounded(0, "foo", 0.1) == "0.0 foo"s);
  CHECK(d.formatDimensionRounded(0, "foo", 0.01) == "0.00 foo"s);
  CHECK(d.formatDimensionRounded(1.25, "foo", 0.02) == "1.26 foo"s);

  CHECK(d.formatDimensionRounded(0.000001, "foo", 0.1) == "0.0 foo"s);
  CHECK(d.formatDimensionRounded(0.00001, "foo", 0.1) == "0.0 foo"s);
  CHECK(d.formatDimensionRounded(0.0001, "foo", 0.1) == "0.0 foo"s);
  CHECK(d.formatDimensionRounded(0.001, "foo", 0.1) == "0.0 foo"s);
  CHECK(d.formatDimensionRounded(0.01, "foo", 0.1) == "0.0 foo"s);
  CHECK(d.formatDimensionRounded(0.1, "foo", 0.1) == "0.1 foo"s);
  CHECK(d.formatDimensionRounded(1.000001, "foo", 0.1) == "1.0 foo"s);
  CHECK(d.formatDimensionRounded(1.200001, "foo", 0.1) == "1.2 foo"s);
  CHECK(d.formatDimensionRounded(1.230001, "foo", 0.1) == "1.2 foo"s);
  CHECK(d.formatDimensionRounded(1.246001, "foo", 0.1) == "1.2 foo"s);
  CHECK(d.formatDimensionRounded(10.000001, "foo", 0.1) == "10.0 foo"s);
  CHECK(d.formatDimensionRounded(123.000001, "foo", 0.1) == "123.0 foo"s);
  CHECK(d.formatDimensionRounded(1234, "foo", 0.1) == "1234.0 foo"s);
  CHECK(d.formatDimensionRounded(1234567, "foo", 0.1) == "1234567.0 foo"s);

  CHECK(d.formatDimensionRounded(-0.000001, "foo", 0.1) == "0.0 foo");
  CHECK(d.formatDimensionRounded(-0.00001, "foo", 0.1) == "0.0 foo");
  CHECK(d.formatDimensionRounded(-0.0001, "foo", 0.1) == "0.0 foo");
  CHECK(d.formatDimensionRounded(-0.001, "foo", 0.1) == "0.0 foo");
  CHECK(d.formatDimensionRounded(-0.01, "foo", 0.1) == "0.0 foo");
  CHECK(d.formatDimensionRounded(-0.1, "foo", 0.1) == "-0.1 foo");
  CHECK(d.formatDimensionRounded(-1.000001, "foo", 0.1) == "-1.0 foo");
  CHECK(d.formatDimensionRounded(-1.200001, "foo", 0.1) == "-1.2 foo");
  CHECK(d.formatDimensionRounded(-1.230001, "foo", 0.1) == "-1.2 foo");
  CHECK(d.formatDimensionRounded(-1.246001, "foo", 0.1) == "-1.2 foo");
  CHECK(d.formatDimensionRounded(-10.000001, "foo", 0.1) == "-10.0 foo");
  CHECK(d.formatDimensionRounded(-123.000001, "foo", 0.1) == "-123.0 foo");
  CHECK(d.formatDimensionRounded(-1234, "foo", 0.1) == "-1234.0 foo");
  CHECK(d.formatDimensionRounded(-1234567, "foo", 0.1) == "-1234567.0 foo");
}
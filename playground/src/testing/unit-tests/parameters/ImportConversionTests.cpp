#include <testing/TestHelper.h>
#include <third-party/include/catch.hpp>
#include <parameters/scale-converters/Linear70DbScaleConverter.h>
#include "parameters/ParameterImportConversions.h"

TEST_CASE("FB Drive 50db to 70db")
{
  auto dimension = ScaleConverter::get<Linear70DbScaleConverter>();
  auto& converter = ParameterImportConversions::get();

  auto testConversion = [&](double in, double expected) {
    auto converted = dimension->controlPositionToDisplay(converter.convert(164, in, 5));
    REQUIRE(converted == Approx(expected));
  };

  testConversion(0, 20);
  testConversion(1, 70);
  testConversion(0.5, 45);
}
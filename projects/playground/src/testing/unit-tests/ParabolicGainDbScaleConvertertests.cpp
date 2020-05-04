#include "testing/TestHelper.h"
#include <catch.hpp>
#include "parameters/scale-converters/ParabolicGainDbScaleConverter.h"

TEST_CASE("to display value and back")
{
  ParabolicGainDbScaleConverter converter;

  auto convert = [&](double cp) {
    auto display = converter.controlPositionToDisplay(cp);
    auto cpFromDisplay = converter.displayToControlPosition(display);
    auto displayFromCP = converter.controlPositionToDisplay(cpFromDisplay);

    REQUIRE(cp == cpFromDisplay);
    REQUIRE(display == displayFromCP);
  };

  convert(0.0);
  convert(0.126);
  convert(0.187);
  convert(1.0);
  convert(0.5);
  convert(0.001);
  convert(0.9901);
  convert(0.7201);
}
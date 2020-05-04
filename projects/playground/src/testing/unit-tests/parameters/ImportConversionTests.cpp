#include <testing/TestHelper.h>
#include <catch.hpp>
#include <parameters/scale-converters/Linear70DbScaleConverter.h>
#include <parameters/scale-converters/ParabolicGainDbScaleConverter.h>
#include "parameters/ParameterImportConversions.h"

TEST_CASE("FB Drive 50db to 70db", "[Parameter][Convert]")
{
  auto newDimension = ScaleConverter::get<Linear70DbScaleConverter>();
  auto &converter = ParameterImportConversions::get();

  using tTests = std::vector<std::pair<double, double>>;
  for(auto testPair : tTests { { 0.0, 20.0 }, { 0.01, 20.5 }, { 0.04, 22.0 }, { 0.1, 25.0 }, { 0.12, 26.0 } })
  {
    auto oldCP = testPair.first;
    auto expectedDV = testPair.second;
    auto newCP = converter.convert(164, oldCP, 5, SoundType::Invalid);
    CHECK(newDimension->controlPositionToDisplay(newCP) == Approx(expectedDV));
  }
}

TEST_CASE("Volume Scaleconverter Display <-> CP")
{
  ParabolicGainDbScaleConverter converter;

  for(auto v = 0; v <= 1000; v += 1)
    CHECK(Approx(v / 1000.0) == converter.displayToControlPosition(converter.controlPositionToDisplay(v / 1000.0)));
}

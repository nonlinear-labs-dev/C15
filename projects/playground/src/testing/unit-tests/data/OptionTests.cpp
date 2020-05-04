#include <catch.hpp>
#include <device-settings/EditSmoothingTime.h>
#include <testing/settings/TestSettings.h>
#include <testing/TestHelper.h>

TEST_CASE("Edit Smoothing Time - Scales")
{
  TestSettings settings;
  EditSmoothingTime time(settings);
  REQUIRE(time.getParent() != nullptr);
}

TEST_CASE("Edit Smoothing Time - To String")
{
  TestSettings settings;
  EditSmoothingTime time(settings);

  SECTION("0.000 ms")
  {
    time.set(0.0);
    REQUIRE(time.getDisplayString() == "0.000 ms");
  }
}
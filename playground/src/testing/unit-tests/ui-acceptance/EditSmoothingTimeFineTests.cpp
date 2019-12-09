#include <third-party/include/catch.hpp>
#include <testing/TestHelper.h>
#include <device-settings/EditSmoothingTime.h>

template <typename tSetting, typename tStr>
inline void AssertControlPositionToDisplayString(double value, const tStr& display)
{
  auto setting = Application::get().getSettings()->getSetting<tSetting>();
  REQUIRE(setting.get());
  setting->set(value);
  REQUIRE(setting->getDisplayString() == display);
}

TEST_CASE("Edit Smoothing Time to Display-String")
{
  AssertControlPositionToDisplayString<EditSmoothingTime>(0.0, "0 ms");
  AssertControlPositionToDisplayString<EditSmoothingTime>(0.5, "100 ms");
  AssertControlPositionToDisplayString<EditSmoothingTime>(1.0, "200 ms");

  AssertControlPositionToDisplayString<EditSmoothingTime>(0.0005, "0.1 ms");
  AssertControlPositionToDisplayString<EditSmoothingTime>(0.0010, "0.2 ms");
  AssertControlPositionToDisplayString<EditSmoothingTime>(0.0015, "0.3 ms");
  AssertControlPositionToDisplayString<EditSmoothingTime>(0.0045, "0.9 ms");

  AssertControlPositionToDisplayString<EditSmoothingTime>(0.005, "1 ms");
  AssertControlPositionToDisplayString<EditSmoothingTime>(0.01, "2 ms");
  AssertControlPositionToDisplayString<EditSmoothingTime>(0.0115, "2.3 ms");
  AssertControlPositionToDisplayString<EditSmoothingTime>(0.0055, "1.1 ms");

  AssertControlPositionToDisplayString<EditSmoothingTime>(0.05, "10 ms");
  AssertControlPositionToDisplayString<EditSmoothingTime>(0.1, "20 ms");
  AssertControlPositionToDisplayString<EditSmoothingTime>(0.055, "11 ms");
  AssertControlPositionToDisplayString<EditSmoothingTime>(0.0555, "11.1 ms");

  AssertControlPositionToDisplayString<EditSmoothingTime>(0.52, "104 ms");
  AssertControlPositionToDisplayString<EditSmoothingTime>(0.995, "199 ms");
  AssertControlPositionToDisplayString<EditSmoothingTime>(0.5215, "104.3 ms");
  AssertControlPositionToDisplayString<EditSmoothingTime>(0.9955, "199.1 ms");
}
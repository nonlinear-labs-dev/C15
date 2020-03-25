#include <testing/TestHelper.h>
#include <parameters/value/Hysteresis.h>

TEST_CASE("Hysteresis Set", "[Parameter]")
{
  Hysteresis h(0.0, 0.1);
  CHECK(h.set(0.01) == 0.0);
  CHECK(h.set(-0.01) == 0.0);
  CHECK(h.set(0.05) == 0.05);
  CHECK(h.set(-0.04) == 0.05);
  CHECK(h.set(-0.05) == -0.05);
  CHECK(h.set(-0.125) == -0.125);
  CHECK(h.set(-0.15) == -0.15);
  CHECK(h.set(-0.125) == -0.15);
}
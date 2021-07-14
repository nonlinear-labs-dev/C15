#include <catch.hpp>
#include <testing/TestHelper.h>
#include <proxies/hwui/base-unit/Ribbon.h>
#include <proxies/hwui/base-unit/UpperRibbon.h>

TEST_CASE("Upper Ribbon", "[HWUI][Ribbon]")
{
  class TestRibbon : public UpperRibbon
  {
   public:
    TestRibbon()
    {
    }

    void setLEDsForValueUniPolar(double value)
    {
      Ribbon::setLEDsForValueUniPolar(value);
    }

    void setLEDsUniPolar(int val)
    {
      Ribbon::setLEDsUniPolar(val);
    }

    void setLEDsForValueBiPolar(double val)
    {
      Ribbon::setLEDsForValueBiPolar(val);
    }

    void setLEDState(int ledPos, FourStateLED::State state)
    {
      Ribbon::setLEDState(ledPos, state);
    }

    void forEachLED(const std::function<void(int, const FourStateLED& led)> cb)
    {
      for(auto i = 0; i < NUM_LEDS_PER_RIBBON; i++)
        cb(i, *getLED(i));
    }
  };

  TestRibbon r;

  const int centerLED = (NUM_LEDS_PER_RIBBON / 2);

  WHEN("Upper Ribbon Unipolar Value Zero")
  {
    r.setLEDsForValueUniPolar(0.0);

    THEN("ALL LEDs Off")
    {
      r.forEachLED([](auto, const auto& led) { CHECK(led.getState() == 0); });
    }
  }

  WHEN("Upper Ribbon Unipolar Value One")
  {
    r.setLEDsForValueUniPolar(1.0);
    THEN("ALL LEDs On")
    {
      r.forEachLED([](auto, const auto& led) { CHECK(led.getState() == 3); });
    }
  }

  WHEN("Upper Ribbon Unipolar 99")
  {
    r.setLEDsUniPolar(99);
    THEN("All but last LED on")
    {
      r.forEachLED([](auto index, const auto& led) {
        auto isLast = index != NUM_LEDS_PER_RIBBON - 1;
        if(isLast)
          CHECK(led.getState() == 3);
        else
          CHECK(led.getState() < 3);
      });
    }
  }

  WHEN("Upper Ribbon Unipolar 1")
  {
    r.setLEDsUniPolar(1);
    THEN("All but first LED off")
    {
      r.forEachLED([](auto index, const auto& led) {
        auto isFirst = index == 0;
        if(isFirst)
          CHECK(led.getState() > 0);
        else
          CHECK(led.getState() == 0);
      });
    }
  }

  WHEN("Upper Ribbon Unipolar Half")
  {
    r.setLEDsForValueUniPolar(0.5);
    THEN("left half on, middle lit, right half off")
    {
      r.forEachLED([](auto index, const auto& led) {
        auto leftHalf = index < 16;
        auto rightHalf = index > 16;

        if(leftHalf)
        {
          CHECK(led.getState() == 3);
        }
        else if(rightHalf)
        {
          CHECK(led.getState() == 0);
        }
        else
        {
          CHECK(led.getState() == 1);
        }
      });
    }
  }

  WHEN("Upper Ribbon Bipolar Zero")
  {
    r.setLEDsForValueBiPolar(0.0);
    THEN("All LEDs but center off")
    {
      r.forEachLED([centerLED](auto index, const auto& led) {
        if(index == centerLED)
          CHECK(led.getState() == 3);
        else
          CHECK(led.getState() == 0);
      });
    }
  }

  WHEN("Upper Ribbon bipolar +1")
  {
    r.setLEDsForValueBiPolar(1.0);
    THEN("Right side LEDs on")
    {
      r.forEachLED([centerLED](auto index, const auto& led) {
        auto left = index < centerLED;
        auto right = index > centerLED;

        if(left)
          CHECK(led.getState() == 0);
        else if(right || index == centerLED)
          CHECK(led.getState() == 3);
      });
    }
  }

  WHEN("Upper Ribbon bipolar -1")
  {
    r.setLEDsForValueBiPolar(-1.0);
    THEN("Left side LEDs on")
    {
      r.forEachLED([centerLED](auto index, const auto& led) {
        auto left = index < centerLED;
        auto right = index > centerLED;

        if(right)
          CHECK(led.getState() == 0);
        else if(left || index == centerLED)
          CHECK(led.getState() == 3);
      });
    }
  }

  WHEN("Upper Ribbon bipolar +0.5")
  {
    r.setLEDsForValueBiPolar(0.5);
    THEN("half right side of LEDs on")
    {
      r.forEachLED([centerLED](auto index, const auto& led) {
        auto right = index >= centerLED;
        auto rightHalfOn = right && index <= (centerLED + (centerLED / 2));

        if(rightHalfOn || index == centerLED)
          CHECK(led.getState() != 0);
        else
          CHECK(led.getState() == 0);
      });
    }
  }

  WHEN("Upper Ribbon bipolar -0.5")
  {
    r.setLEDsForValueBiPolar(-0.5);
    THEN("half left side of LEDs on")
    {
      r.forEachLED([centerLED](auto index, const auto& led) {
        auto left = index < centerLED;
        auto leftHalfOn = left && index >= (centerLED / 2);

        if(leftHalfOn || index == centerLED)
          CHECK(led.getState() != 0);
        else
          REQUIRE(led.getState() == 0);
      });
    }
  }
}

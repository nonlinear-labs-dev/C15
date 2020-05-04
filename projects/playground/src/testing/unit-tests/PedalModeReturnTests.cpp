#include <catch.hpp>
#include "testing/TestHelper.h"
#include "mock/MockPresetStorage.h"
#include <presets/Bank.h>
#include <presets/Preset.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/HWUI.h>
#include <groups/HardwareSourcesGroup.h>
#include <parameters/RibbonParameter.h>
#include <parameters/ModulationRoutingParameter.h>

SCENARIO("Ribbon Return Mode")
{
  auto eb = TestHelper::getEditBuffer();
  auto ribbon
      = dynamic_cast<RibbonParameter*>(eb->findParameterByID(HardwareSourcesGroup::getUpperRibbonParameterID()));

  REQUIRE(ribbon);

  WHEN("Set Return Mode to center")
  {
    {
      auto scope = TestHelper::createTestScope();
      ribbon->undoableSetRibbonReturnMode(scope->getTransaction(), RibbonReturnMode::RETURN);
    }

    REQUIRE(ribbon->getRibbonReturnMode() == RibbonReturnMode::RETURN);

    THEN("Route to A and B")
    {
      auto rToA = dynamic_cast<ModulationRoutingParameter*>(eb->findParameterByID({ 285, VoiceGroup::Global }));
      auto rToB = dynamic_cast<ModulationRoutingParameter*>(eb->findParameterByID({ 286, VoiceGroup::Global }));

      {
        auto scope = TestHelper::createTestScope();
        rToA->setCPFromHwui(scope->getTransaction(), 0.1);
        rToB->setCPFromHwui(scope->getTransaction(), 0.2);
      }

      REQUIRE_FALSE(rToA->getValue().isBoolean());
      REQUIRE_FALSE(rToB->getValue().isBoolean());

      WHEN("Set Return Mode to None")
      {
        {
          auto scope = TestHelper::createTestScope();
          ribbon->undoableSetRibbonReturnMode(scope->getTransaction(), RibbonReturnMode::STAY);
        }

        REQUIRE(rToA->getValue().isBoolean());
        REQUIRE(rToB->getValue().isBoolean());

        REQUIRE(rToA->getValue().getRawValue() == 0.0);  //Gets reset to 0
        REQUIRE(rToB->getValue().getRawValue() == 0.2);  //stays at 0.2 but appears bidirectional as it is boolean value
      }
    }
  }
}
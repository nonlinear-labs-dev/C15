#include <catch.hpp>
#include "testing/TestHelper.h"
#include "mock/MockPresetStorage.h"
#include <presets/Bank.h>
#include <presets/Preset.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/HWUI.h>
#include <parameters/ModulateableParameter.h>

SCENARIO("Single sound with modulation aspects on mod-param")
{
  MockPresetStorage storage;

  auto eb = TestHelper::getEditBuffer();
  auto modParamI = dynamic_cast<ModulateableParameter*>(eb->findParameterByID({ 0, VoiceGroup::I }));
  auto modParamII = dynamic_cast<ModulateableParameter*>(eb->findParameterByID({ 0, VoiceGroup::II }));

  {
    auto scope = TestHelper::createTestScope();
    eb->undoableLoad(scope->getTransaction(), storage.getSinglePreset(), true);
    eb->undoableInitSound(scope->getTransaction(), Defaults::FactoryDefault);
  }

  REQUIRE(modParamI);
  REQUIRE(modParamII);
  REQUIRE(eb->getType() == SoundType::Single);

  WHEN("modulation is assigned")
  {
    {
      auto scope = TestHelper::createTestScope();
      modParamI->setModulationSource(scope->getTransaction(), MacroControls::MC5);
      modParamI->setModulationAmount(scope->getTransaction(), 0.5);
    }

    REQUIRE(modParamI->getModulationSource() == MacroControls::MC5);

    WHEN("sound is converted to layer")
    {
      {
        auto scope = TestHelper::createTestScope();
        eb->undoableConvertToDual(scope->getTransaction(), SoundType::Layer);
      }

      REQUIRE(eb->getType() == SoundType::Layer);

      THEN("Macro aspects have been copied to VG II")
      {
        REQUIRE(modParamI->getModulationSource() == MacroControls::MC5);
        REQUIRE(modParamI->getModulationAmount() == 0.5);
        REQUIRE(modParamII->getModulationSource() == MacroControls::MC5);
        REQUIRE(modParamII->getModulationAmount() == 0.5);
      }
    }

    WHEN("sound is converted to split")
    {
      {
        auto scope = TestHelper::createTestScope();
        eb->undoableConvertToDual(scope->getTransaction(), SoundType::Split);
      }

      REQUIRE(eb->getType() == SoundType::Split);

      THEN("Macro aspects have been copied to VG II")
      {
        REQUIRE(modParamI->getModulationSource() == MacroControls::MC5);
        REQUIRE(modParamI->getModulationAmount() == 0.5);
        REQUIRE(modParamII->getModulationSource() == MacroControls::MC5);
        REQUIRE(modParamII->getModulationAmount() == 0.5);
      }
    }
  }
}

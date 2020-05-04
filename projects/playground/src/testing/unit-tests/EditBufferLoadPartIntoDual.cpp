#include <testing/unit-tests/mock/MockPresetStorage.h>
#include "testing/TestHelper.h"
#include <catch.hpp>
#include <presets/Preset.h>
#include <presets/PresetParameter.h>

TEST_CASE("Load Dual Part into Part")
{
  MockPresetStorage presets;
  auto editBuffer = TestHelper::getEditBuffer();

  auto splitPreset = presets.getSplitPreset();

  auto presetValue = 0.1;
  //Prepare Presets
  {
    auto scope = TestHelper::createTestScope();
    auto transaction = scope->getTransaction();

    splitPreset->forEachParameter([&](PresetParameter* pp) { pp->setValue(transaction, presetValue); });
  }

  constexpr auto parameterNumber = 0;

  WHEN("Init Sound is Split")
  {
    {
      auto scope = TestHelper::createTestScope();
      editBuffer->undoableConvertToDual(scope->getTransaction(), SoundType::Split);
      editBuffer->undoableInitSound(scope->getTransaction());

      REQUIRE(editBuffer->getType() == SoundType::Split);
      REQUIRE_FALSE(editBuffer->findAnyParameterChanged());
    }

    THEN("Load Split Part I into I")
    {
      editBuffer->undoableLoadToPart(splitPreset, VoiceGroup::I, VoiceGroup::I);
      REQUIRE(editBuffer->findParameterByID({ parameterNumber, VoiceGroup::I })->getControlPositionValue()
              == presetValue);
    }

    THEN("Load Split Part I into II")
    {
      editBuffer->undoableLoadToPart(splitPreset, VoiceGroup::I, VoiceGroup::II);
      REQUIRE(editBuffer->findParameterByID({ parameterNumber, VoiceGroup::II })->getControlPositionValue()
              == presetValue);
    }

    THEN("Load Split Part II into I")
    {
      editBuffer->undoableLoadToPart(splitPreset, VoiceGroup::II, VoiceGroup::I);
      REQUIRE(editBuffer->findParameterByID({ parameterNumber, VoiceGroup::I })->getControlPositionValue()
              == presetValue);
    }

    THEN("Load Split Part II into II")
    {
      editBuffer->undoableLoadToPart(splitPreset, VoiceGroup::II, VoiceGroup::II);
      REQUIRE(editBuffer->findParameterByID({ parameterNumber, VoiceGroup::II })->getControlPositionValue()
              == presetValue);
    }
  }
}

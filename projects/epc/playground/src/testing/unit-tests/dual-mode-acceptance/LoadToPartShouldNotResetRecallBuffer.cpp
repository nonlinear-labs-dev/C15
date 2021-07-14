#include <testing/TestHelper.h>
#include <testing/unit-tests/mock/MockPresetStorage.h>
#include <parameter_declarations.h>
#include <presets/Preset.h>
#include <presets/PresetParameter.h>

TEST_CASE("Load to Part should lead to changed star and recall buffer unchanged")
{

  MockPresetStorage presets;
  auto singlePreset = presets.getSinglePreset();
  auto layerPreset = presets.getLayerPreset();
  auto splitPreset = presets.getSplitPreset();

  auto preparePreset = [](auto transaction, Preset* preset, VoiceGroup vg) {
    preset->findParameterByID({ C15::PID::Env_B_Att, vg }, true)->setValue(transaction, 0.187);
  };

  //Prepare Presets to be different from current EB
  {
    auto scope = TestHelper::createTestScope();
    auto transaction = scope->getTransaction();
    preparePreset(transaction, singlePreset, VoiceGroup::I);
    preparePreset(transaction, layerPreset, VoiceGroup::I);
    preparePreset(transaction, layerPreset, VoiceGroup::II);
    preparePreset(transaction, splitPreset, VoiceGroup::I);
    preparePreset(transaction, splitPreset, VoiceGroup::II);
  }

  auto eb = TestHelper::getEditBuffer();
  EditBufferUseCases ebUseCases(eb);

  auto loadPresetAndCheck = [&](auto preset, auto from, auto to) {
    ebUseCases.undoableLoadToPart(preset, from, to);
    CHECK(eb->findAnyParameterChanged());
  };

  WHEN("EB is Split")
  {
    TestHelper::initDualEditBuffer<SoundType::Split>();
    WHEN("Single Preset loaded into I")
    {
      loadPresetAndCheck(singlePreset, VoiceGroup::I, VoiceGroup::I);
    }

    WHEN("Single Preset loaded into II")
    {
      loadPresetAndCheck(singlePreset, VoiceGroup::I, VoiceGroup::II);
    }

    WHEN("Split Preset loaded into I")
    {
      loadPresetAndCheck(splitPreset, VoiceGroup::I, VoiceGroup::I);
    }

    WHEN("Split Preset loaded into II")
    {
      loadPresetAndCheck(splitPreset, VoiceGroup::I, VoiceGroup::II);
    }

    WHEN("Layer Preset loaded into I")
    {
      loadPresetAndCheck(layerPreset, VoiceGroup::I, VoiceGroup::I);
    }

    WHEN("Layer Preset loaded into II")
    {
      loadPresetAndCheck(layerPreset, VoiceGroup::I, VoiceGroup::II);
    }
  }

  WHEN("EB is Layer")
  {
    TestHelper::initDualEditBuffer<SoundType::Layer>();
    WHEN("Single Preset loaded into I")
    {
      loadPresetAndCheck(singlePreset, VoiceGroup::I, VoiceGroup::I);
    }

    WHEN("Single Preset loaded into II")
    {
      loadPresetAndCheck(singlePreset, VoiceGroup::I, VoiceGroup::II);
    }

    WHEN("Split Preset loaded into I")
    {
      loadPresetAndCheck(splitPreset, VoiceGroup::I, VoiceGroup::I);
    }

    WHEN("Split Preset loaded into II")
    {
      loadPresetAndCheck(splitPreset, VoiceGroup::I, VoiceGroup::II);
    }

    WHEN("Layer Preset loaded into I")
    {
      loadPresetAndCheck(layerPreset, VoiceGroup::I, VoiceGroup::I);
    }

    WHEN("Layer Preset loaded into II")
    {
      loadPresetAndCheck(layerPreset, VoiceGroup::I, VoiceGroup::II);
    }
  }
}
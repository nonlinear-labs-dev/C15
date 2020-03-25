#include <third-party/include/catch.hpp>
#include <testing/TestHelper.h>

#include <presets/EditBuffer.h>
#include <parameters/ModulateableParameter.h>
#include <proxies/audio-engine/AudioEngineProxy.h>
#include <nltools/Types.h>
#include <presets/Bank.h>
#include <presets/Preset.h>
#include <presets/PresetParameter.h>
#include <testing/unit-tests/mock/MockPresetStorage.h>

bool isException(const Parameter *p)
{
  auto num = p->getID().getNumber();
  return num == 248 || num == 249 || num == 358 || num == 360;
}

template <SoundType tType, VoiceGroup tLoadToVoiceGroup>
void LoadSinglePresetWithValueIntoInitDualSound(Preset *preset, tControlPositionValue paramValue)
{
  WHEN("converted to " + toString(tType) + " (" + toString(tLoadToVoiceGroup) + ")")
  {
    static_assert(tType == SoundType::Split || tType == SoundType::Layer, "");

    auto editBuffer = TestHelper::getEditBuffer();

    auto presetMasterVolume = preset->findParameterByID({ 247, VoiceGroup::Global }, true);
    auto presetMasterTune = preset->findParameterByID({ 248, VoiceGroup::Global }, true);

    auto invert = [](VoiceGroup v) { return v == VoiceGroup::I ? VoiceGroup::II : VoiceGroup::I; };

    auto otherPartVolume = editBuffer->findParameterByID({ 358, invert(tLoadToVoiceGroup) });
    auto otherPartTune = editBuffer->findParameterByID({ 360, invert(tLoadToVoiceGroup) });

    tControlPositionValue oldOtherPartVolume;
    tControlPositionValue oldOtherPartTune;

    {
      auto scope = TestHelper::createTestScope();
      TestHelper::initDualEditBuffer<tType>();

      preset->forEachParameter([&](PresetParameter *pp) { pp->setValue(scope->getTransaction(), paramValue); });
      presetMasterVolume->setValue(scope->getTransaction(), paramValue / 2.0);
      presetMasterTune->setValue(scope->getTransaction(), paramValue / 2.0);

      oldOtherPartVolume = otherPartVolume->getControlPositionValue();
      oldOtherPartTune = otherPartTune->getControlPositionValue();

      editBuffer->undoableLoadPresetIntoDualSound(scope->getTransaction(), preset, tLoadToVoiceGroup);
    }

    THEN(toString(tLoadToVoiceGroup) + " has preset values")
    {
      editBuffer->forEachParameter<tLoadToVoiceGroup>([&](Parameter *p) {
        if(!isException(p))
          CHECK_PARAMETER_CP_EQUALS_FICTION(p, paramValue);
      });
    }

    THEN(toString(tLoadToVoiceGroup) + " part master got loaded into preset global master")
    {
      auto partVolume = editBuffer->findParameterByID({ 358, tLoadToVoiceGroup });
      auto partTune = editBuffer->findParameterByID({ 360, tLoadToVoiceGroup });

      CHECK_PARAMETER_CP_EQUALS_FICTION(partVolume, presetMasterVolume->getValue());
      CHECK_PARAMETER_CP_EQUALS_FICTION(partTune, presetMasterTune->getValue());
    }

    THEN(toString(invert(tLoadToVoiceGroup)) + " part master was not touched")
    {
      CHECK_PARAMETER_CP_EQUALS_FICTION(otherPartVolume, oldOtherPartVolume);
      CHECK_PARAMETER_CP_EQUALS_FICTION(otherPartTune, oldOtherPartTune);
    }
  }
}

TEST_CASE("Load Single into Dual Part", "[EditBuffer][Loading]")
{
  MockPresetStorage presets;
  auto preset = presets.getSinglePreset();

  for(auto val : std::vector<double>{ 0.5 })
  {
    WHEN("Test with value: " + std::to_string(val))
    {
      LoadSinglePresetWithValueIntoInitDualSound<SoundType::Layer, VoiceGroup::I>(preset, val);
      LoadSinglePresetWithValueIntoInitDualSound<SoundType::Split, VoiceGroup::I>(preset, val);
      LoadSinglePresetWithValueIntoInitDualSound<SoundType::Layer, VoiceGroup::II>(preset, val);
      LoadSinglePresetWithValueIntoInitDualSound<SoundType::Split, VoiceGroup::II>(preset, val);
    }
  }
}

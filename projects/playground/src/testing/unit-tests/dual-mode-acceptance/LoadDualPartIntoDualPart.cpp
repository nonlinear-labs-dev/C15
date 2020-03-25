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
#include <parameters/scale-converters/ParabolicGainDbScaleConverter.h>

inline bool isException(const Parameter *p)
{
  auto num = p->getID().getNumber();
  return num == 358 || num == 360;
}

template <SoundType tType, VoiceGroup tLoadFromPart, VoiceGroup tLoadToVoiceGroup>
void LoadDualPresetPartWithValueIntoInitDualSoundPart(Preset *preset, tControlPositionValue paramValue)
{
  static_assert(tType == SoundType::Split || tType == SoundType::Layer, "");

  constexpr auto globalValue = 0.75;

  auto editBuffer = TestHelper::getEditBuffer();

  {
    auto scope = TestHelper::createTestScope();
    TestHelper::initDualEditBuffer<tType>();

    preset->forEachParameter([&](PresetParameter *pp) { pp->setValue(scope->getTransaction(), paramValue); });

    editBuffer->forEachParameter<VoiceGroup::Global>(
        [&](Parameter *p) { p->setCPFromHwui(scope->getTransaction(), globalValue); });

    editBuffer->undoableLoadPresetPartIntoPart(scope->getTransaction(), preset, tLoadFromPart, tLoadToVoiceGroup);
  }

  auto presetPartVolume = preset->findParameterByID({ 358, tLoadFromPart }, true);
  auto presetPartTune = preset->findParameterByID({ 360, tLoadFromPart }, true);
  auto presetGlobalVolume = preset->findParameterByID({ 247, VoiceGroup::Global }, true);
  auto presetGlobalTune = preset->findParameterByID({ 248, VoiceGroup::Global }, true);

  auto partVolume = editBuffer->findParameterByID({ 358, tLoadToVoiceGroup });
  auto partTune = editBuffer->findParameterByID({ 360, tLoadToVoiceGroup });

  editBuffer->forEachParameter<tLoadToVoiceGroup>([&](Parameter *p) {
    if(!isException(p))
    {
      CHECK_PARAMETER_CP_EQUALS_FICTION(p, paramValue)
    }
    else if(p->getID().getNumber() == 358)
    {
      double displayValueExpected = 0.0;

      if(paramValue == 0)
        displayValueExpected = -128;
      else if(paramValue == 0.5)
        displayValueExpected = 0;
      else if(paramValue == 1.0)
        displayValueExpected = 12;

      REQUIRE(partVolume->getDisplayValue() == Approx(displayValueExpected).margin(0.05));
    }
    else if(p->getID().getNumber() == 360)
    {
      CHECK_PARAMETER_CP_EQUALS_FICTION(partTune, presetPartTune->getValue() + presetGlobalTune->getValue())
    }
  });

  editBuffer->forEachParameter<VoiceGroup::Global>(
      [&](Parameter *p) { CHECK_PARAMETER_CP_EQUALS_FICTION(p, globalValue); });
}

TEST_CASE("Load Dual Part into Dual Part", "[EditBuffer][Loading]")
{
  MockPresetStorage presets;
  auto layerpreset = presets.getLayerPreset();

  for(auto val : std::vector<double>{ 0.5, 0.0, 1.0 })
  {
    WHEN("Test with value: " + std::to_string(val))
    {
      LoadDualPresetPartWithValueIntoInitDualSoundPart<SoundType::Layer, VoiceGroup::I, VoiceGroup::I>(layerpreset,
                                                                                                       val);

      LoadDualPresetPartWithValueIntoInitDualSoundPart<SoundType::Layer, VoiceGroup::I, VoiceGroup::II>(layerpreset,
                                                                                                        val);

      LoadDualPresetPartWithValueIntoInitDualSoundPart<SoundType::Layer, VoiceGroup::II, VoiceGroup::I>(layerpreset,
                                                                                                        val);

      LoadDualPresetPartWithValueIntoInitDualSoundPart<SoundType::Layer, VoiceGroup::II, VoiceGroup::II>(layerpreset,
                                                                                                         val);
    }
  }
}

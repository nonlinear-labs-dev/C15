#include <third-party/include/catch.hpp>
#include <testing/TestHelper.h>

#include <presets/EditBuffer.h>
#include <parameters/ModulateableParameter.h>
#include <proxies/audio-engine/AudioEngineProxy.h>
#include <nltools/Types.h>
#include <presets/Bank.h>
#include <presets/Preset.h>
#include <presets/PresetParameter.h>

template <SoundType tType> void ConvertToDualInitializesPartsFromVoiceGroupI(tControlPositionValue paramValue)
{
  const auto isException = [](const Parameter *p) -> bool {
    auto num = p->getID().getNumber();
    //toFX, part Volume, part Tune, unison voices, fade From
    return num == 362 || num == 358 || num == 360 || num == 249 || num == 396;
  };

  WHEN("converted to " + toString(tType))
  {
    static_assert(tType == SoundType::Split || tType == SoundType::Layer, "");

    auto editBuffer = TestHelper::getEditBuffer();

    auto scope = TestHelper::createTestScope();
    TestHelper::initSingleEditBuffer(scope->getTransaction());

    editBuffer->forEachParameter<VoiceGroup::I>(
        [&](Parameter *p) { p->setCPFromHwui(scope->getTransaction(), paramValue); });

    editBuffer->forEachParameter<VoiceGroup::II>([&](Parameter *p) { p->setCPFromHwui(scope->getTransaction(), 0); });

    const auto unisonDetune = editBuffer->findParameterByID({ 250, VoiceGroup::I })->getControlPositionValue();
    const auto unisonPhase = editBuffer->findParameterByID({ 252, VoiceGroup::I })->getControlPositionValue();
    const auto unisonPan = editBuffer->findParameterByID({ 253, VoiceGroup::I })->getControlPositionValue();

    const auto monoEnable = editBuffer->findParameterByID({ 364, VoiceGroup::I })->getControlPositionValue();
    const auto monoPrio = editBuffer->findParameterByID({ 366, VoiceGroup::I })->getControlPositionValue();
    const auto monoLegato = editBuffer->findParameterByID({ 367, VoiceGroup::I })->getControlPositionValue();
    const auto monoGlide = editBuffer->findParameterByID({ 365, VoiceGroup::I })->getControlPositionValue();

    editBuffer->undoableConvertToDual(scope->getTransaction(), tType);

    for(auto &vg : { VoiceGroup::I, VoiceGroup::II })
    {
      THEN(toString(vg) + " has VG I parameter values")
      {

        const auto vgDetune = editBuffer->findParameterByID({ 250, vg })->getControlPositionValue();
        const auto vgPhase = editBuffer->findParameterByID({ 252, vg })->getControlPositionValue();
        const auto vgPan = editBuffer->findParameterByID({ 253, vg })->getControlPositionValue();

        const auto vgEnable = editBuffer->findParameterByID({ 364, vg })->getControlPositionValue();
        const auto vgPrio = editBuffer->findParameterByID({ 366, vg })->getControlPositionValue();
        const auto vgLegato = editBuffer->findParameterByID({ 367, vg })->getControlPositionValue();
        const auto vgGlide = editBuffer->findParameterByID({ 365, vg })->getControlPositionValue();

        const auto toFX = editBuffer->findParameterByID({ 362, vg })->getControlPositionValue();
        const auto toFXDefault = editBuffer->findParameterByID({ 362, vg })->getDefaultValue();

        editBuffer->forEachParameter(vg, [&](Parameter *p) {
          if(!isException(p))
          {
            THEN(p->getLongName() + " got: " + std::to_string(p->getControlPositionValue())
                 + " expected: " + std::to_string(paramValue))
            {
              CHECK_PARAMETER_CP_EQUALS_FICTION(p, paramValue);
            }
          }
        });

        REQUIRE(toFX == toFXDefault);

        REQUIRE(unisonDetune == vgDetune);
        REQUIRE(unisonPhase == vgPhase);
        REQUIRE(unisonPan == vgPan);

        REQUIRE(monoEnable == vgEnable);
        REQUIRE(monoPrio == vgPrio);
        REQUIRE(monoLegato == vgLegato);
        REQUIRE(monoGlide == vgGlide);
      }
    }
  }
}

TEST_CASE("Convert Single to Dual Sound", "[EditBuffer][Convert]")
{
  for(auto val : std::vector<double> { 0.187, 0.845, 0.34, 0, 1, 0.94 })
  {
    WHEN("Test with value: " + std::to_string(val))
    {
      ConvertToDualInitializesPartsFromVoiceGroupI<SoundType::Layer>(val);
      ConvertToDualInitializesPartsFromVoiceGroupI<SoundType::Split>(val);
    }
  }
}

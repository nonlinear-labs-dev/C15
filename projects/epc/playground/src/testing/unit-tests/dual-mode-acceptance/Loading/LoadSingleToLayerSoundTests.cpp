#include <iostream>
#include <testing/unit-tests/mock/MockPresetStorage.h>
#include <parameters/scale-converters/ParabolicGainDbScaleConverter.h>
#include <parameter_declarations.h>
#include "testing/TestHelper.h"
#include "testing/unit-tests/mock/EditBufferNamedLogicalParts.h"
#include "proxies/hwui/HWUI.h"
#include "presets/Preset.h"
#include "presets/PresetParameter.h"
#include "parameters/SplitPointParameter.h"
#include "parameters/scale-converters/ScaleConverter.h"

using EBL = EditBufferLogicalParts;

TEST_CASE("Load Single into Layer Part I")
{
  auto eb = TestHelper::getEditBuffer();
  EditBufferUseCases ebUseCases(eb);
  MockPresetStorage presets;
  auto preset = presets.getSinglePreset();

  REQUIRE(preset->getType() == SoundType::Single);

  {
    auto scope = TestHelper::createTestScope();
    auto transaction = scope->getTransaction();
    TestHelper::initDualEditBuffer<SoundType::Layer>(transaction);
    Application::get().getHWUI()->setCurrentVoiceGroup(VoiceGroup::I);

    auto envAAttack = preset->findParameterByID({ 0, VoiceGroup::I }, true);
    envAAttack->setValue(transaction, 0.666);

    auto volume = preset->findParameterByID({ 247, VoiceGroup::Global }, true);
    volume->setValue(transaction, 0.265);
    volume->setField(transaction, PresetParameter::Fields::ModSource, "1");
    volume->setField(transaction, PresetParameter::Fields::ModAmount, "1");

    auto unisonVoices = preset->findParameterByID({ 249, VoiceGroup::I }, true);
    unisonVoices->setValue(transaction, 1);  // <- setting cp in range 0..23

    preset->setName(transaction, "Hello Single");

    TestHelper::changeAllParameters(transaction);
  }

  WHEN("Load")
  {
    const auto localSpecialIIHash
        = EBL::createValueHash(EBL::getCrossFB<VoiceGroup::II>(), EBL::getToFX<VoiceGroup::II>());
    const auto toFXIHash = EBL::createValueHash(EBL::getToFX<VoiceGroup::I>());
    const auto oldVolumeDisplay = EBL::getMasterVolume()->getDisplayValue();
    const auto oldTuneDisplay = EBL::getMasterTune()->getDisplayValue();
    const auto oldMasterHash = EBL::createHashOfVector(EBL::getMaster());
    const auto oldScaleHash = EBL::createHashOfVector(EBL::getScale());
    const auto oldMCMHash = EBL::createHashOfVector(EBL::getModMatrix());
    const auto oldFadeIHash = EBL::createHashOfVector(EBL::getFade<VoiceGroup::I>());
    const auto oldFadeIIHash = EBL::createHashOfVector(EBL::getFade<VoiceGroup::II>());
    const auto oldCrossFBIHash = EBL::createValueHash(EBL::getCrossFB<VoiceGroup::I>());
    const auto oldVoicesIHash = EBL::createHashOfVector(EBL::getVoices<VoiceGroup::I>());

    ebUseCases.undoableLoadToPart(preset, VoiceGroup::I, VoiceGroup::I);

    THEN("Type is Same")
    {
      CHECK(eb->getType() == SoundType::Layer);
    }

    THEN("Voice Group Label was loaded")
    {
      CHECK(eb->getVoiceGroupName(VoiceGroup::I) == "Hello Single");
    }

    THEN("Local Special I/II unchanged")
    {
      CHECK(EBL::createValueHash(EBL::getCrossFB<VoiceGroup::I>()) == oldCrossFBIHash);
      CHECK(EBL::createValueHash(EBL::getToFX<VoiceGroup::I>()) == toFXIHash);
      CHECK(EBL::createValueHash(EBL::getCrossFB<VoiceGroup::II>(), EBL::getToFX<VoiceGroup::II>())
            == localSpecialIIHash);
    }

    THEN("Local Normal was copied to current VG")
    {
      CHECK_PARAMETER_CP_EQUALS_FICTION(eb->findParameterByID({ 0, VoiceGroup::I }), 0.666);
    }

    THEN("Unison and Mono I Untouched")
    {
      CHECK(oldVoicesIHash == EBL::createHashOfVector(EBL::getVoices<VoiceGroup::I>()));
    }

    THEN("Fade I/II is untouched")
    {
      CHECK(EBL::createHashOfVector(EBL::getFade<VoiceGroup::I>()) == oldFadeIHash);
      CHECK(EBL::createHashOfVector(EBL::getFade<VoiceGroup::II>()) == oldFadeIIHash);
    }

    THEN("Unison and Mono II is default")
    {
      CHECK(EBL::isFactoryDefaultLoaded(EBL::getVoices<VoiceGroup::II>()));
    }

    THEN("Part Volume and Tune correct")
    {
      auto volumeScale
          = static_cast<const ParabolicGainDbScaleConverter*>(EBL::getMasterVolume()->getValue().getScaleConverter());
      auto tuneScale = EBL::getMasterTune()->getValue().getScaleConverter();

      PresetParameter* vol = preset->findParameterByID({ 247, VoiceGroup::Global }, true);
      PresetParameter* tune = preset->findParameterByID({ 248, VoiceGroup::Global }, true);

      const auto presetGlobalVolumeDisplay = volumeScale->controlPositionToDisplay(vol->getValue());
      const auto calculatedVolumeCP
          = volumeScale->displayToControlPosition(presetGlobalVolumeDisplay - oldVolumeDisplay);
      CHECK_PARAMETER_CP_EQUALS_FICTION(EBL::getPartVolume<VoiceGroup::I>(), calculatedVolumeCP);

      const auto presetGlobalTuneDisplay = tuneScale->controlPositionToDisplay(tune->getValue());
      const auto newPartTuneDisplay = presetGlobalTuneDisplay - oldTuneDisplay;
      CHECK(EBL::getPartTune<VoiceGroup::I>()->getDisplayValue() == Approx(newPartTuneDisplay));
    }

    THEN("Part Master Modulation transfered")
    {
      CHECK(EBL::getPartVolume<VoiceGroup::I>()->getModulationSource() == MacroControls::MC1);
      CHECK(EBL::getPartVolume<VoiceGroup::I>()->getModulationAmount() == 1);
    }

    THEN("Global Groups unchanged")
    {
      CHECK(EBL::createHashOfVector(EBL::getMaster()) == oldMasterHash);
      CHECK(EBL::createHashOfVector(EBL::getModMatrix()) == oldMCMHash);
      CHECK(EBL::createHashOfVector(EBL::getScale()) == oldScaleHash);
    }
  }
}

TEST_CASE("Load Single into Layer Part II")
{
  auto eb = TestHelper::getEditBuffer();
  EditBufferUseCases ebUseCases(eb);

  MockPresetStorage presets;
  auto preset = presets.getSinglePreset();

  REQUIRE(preset->getType() == SoundType::Single);

  {
    auto scope = TestHelper::createTestScope();
    auto transaction = scope->getTransaction();
    TestHelper::initDualEditBuffer<SoundType::Layer>(transaction);
    Application::get().getHWUI()->setCurrentVoiceGroup(VoiceGroup::II);

    auto envAAttack = preset->findParameterByID({ 0, VoiceGroup::I }, true);
    envAAttack->setValue(transaction, 0.666);

    auto volume = preset->findParameterByID({ 247, VoiceGroup::Global }, true);
    volume->setValue(transaction, 0.265);
    volume->setField(transaction, PresetParameter::Fields::ModSource, "1");
    volume->setField(transaction, PresetParameter::Fields::ModAmount, "1");

    auto unisonVoices = preset->findParameterByID({ 249, VoiceGroup::I }, true);
    unisonVoices->setValue(transaction, 1);  // <- setting cp in range 0..23

    preset->setName(transaction, "Hello Single");

    TestHelper::changeAllParameters(transaction);
  }

  WHEN("Load")
  {
    const auto localSpecialIHash
        = EBL::createValueHash(EBL::getCrossFB<VoiceGroup::I>(), EBL::getToFX<VoiceGroup::I>());
    const auto toFXIIHash = EBL::createValueHash(EBL::getToFX<VoiceGroup::II>());
    const auto oldSplitCP
        = eb->findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::I })->getControlPositionValue();
    const auto oldVolumeDisplay = EBL::getMasterVolume()->getDisplayValue();
    const auto oldTuneDisplay = EBL::getMasterTune()->getDisplayValue();
    const auto oldMasterHash = EBL::createHashOfVector(EBL::getMaster());
    const auto oldScaleHash = EBL::createHashOfVector(EBL::getScale());
    const auto oldMCMHash = EBL::createHashOfVector(EBL::getModMatrix());
    const auto oldFadeIHash = EBL::createHashOfVector(EBL::getFade<VoiceGroup::I>());
    const auto oldFadeIIHash = EBL::createHashOfVector(EBL::getFade<VoiceGroup::II>());
    const auto oldCrossFBIIHash = EBL::createValueHash(EBL::getCrossFB<VoiceGroup::II>());
    const auto oldVoicesIHash = EBL::createHashOfVector(EBL::getVoices<VoiceGroup::I>());

    ebUseCases.undoableLoadToPart(preset, VoiceGroup::I, VoiceGroup::II);

    THEN("Voice Group Label was loaded")
    {
      CHECK(eb->getVoiceGroupName(VoiceGroup::II) == "Hello Single");
    }

    THEN("Type is Same")
    {
      CHECK(eb->getType() == SoundType::Layer);
    }

    THEN("Local Special unchanged")
    {
      CHECK(EBL::createValueHash(EBL::getCrossFB<VoiceGroup::I>(), EBL::getToFX<VoiceGroup::I>()) == localSpecialIHash);
    }

    THEN("Cross FB and to fx II ignored")
    {
      CHECK(EBL::createValueHash(EBL::getCrossFB<VoiceGroup::II>()) == oldCrossFBIIHash);
      CHECK(EBL::createValueHash(EBL::getToFX<VoiceGroup::II>()) == toFXIIHash);
    }

    THEN("Local Normal was copied to current VG")
    {
      CHECK_PARAMETER_CP_EQUALS_FICTION(eb->findParameterByID({ 0, VoiceGroup::II }), 0.666);
    }

    THEN("Split unchanged")
    {
      CHECK_PARAMETER_CP_EQUALS_FICTION(eb->findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::I }),
                                        oldSplitCP);
    }

    THEN("Unison and Mono I Untouched")
    {
      CHECK(oldVoicesIHash == EBL::createHashOfVector(EBL::getVoices<VoiceGroup::I>()));
    }

    THEN("Fade I/II is untouched")
    {
      CHECK(EBL::createHashOfVector(EBL::getFade<VoiceGroup::I>()) == oldFadeIHash);
      CHECK(EBL::createHashOfVector(EBL::getFade<VoiceGroup::II>()) == oldFadeIIHash);
    }

    THEN("Unison and Mono II is default")
    {
      CHECK(EBL::isFactoryDefaultLoaded(EBL::getVoices<VoiceGroup::II>()));
    }

    THEN("Part Volume and Tune correct")
    {
      auto volumeScale
          = static_cast<const ParabolicGainDbScaleConverter*>(EBL::getMasterVolume()->getValue().getScaleConverter());
      auto tuneScale = EBL::getMasterTune()->getValue().getScaleConverter();

      PresetParameter* vol = preset->findParameterByID({ 247, VoiceGroup::Global }, true);
      PresetParameter* tune = preset->findParameterByID({ 248, VoiceGroup::Global }, true);

      const auto presetGlobalVolumeDisplay = volumeScale->controlPositionToDisplay(vol->getValue());
      const auto calculatedVolumeCP
          = volumeScale->displayToControlPosition(presetGlobalVolumeDisplay - oldVolumeDisplay);
      CHECK_PARAMETER_CP_EQUALS_FICTION(EBL::getPartVolume<VoiceGroup::II>(), calculatedVolumeCP);

      const auto presetGlobalTuneDisplay = tuneScale->controlPositionToDisplay(tune->getValue());
      const auto newPartTuneDisplay = presetGlobalTuneDisplay - oldTuneDisplay;
      CHECK(EBL::getPartTune<VoiceGroup::II>()->getDisplayValue() == Approx(newPartTuneDisplay));
    }

    THEN("Part Master Modulation Aspects copied")
    {
      CHECK(EBL::getPartVolume<VoiceGroup::II>()->getModulationSource() == MacroControls::MC1);
      CHECK(EBL::getPartVolume<VoiceGroup::II>()->getModulationAmount() == 1);
    }

    THEN("Global Groups unchanged")
    {
      CHECK(EBL::createHashOfVector(EBL::getMaster()) == oldMasterHash);
      CHECK(EBL::createHashOfVector(EBL::getModMatrix()) == oldMCMHash);
      CHECK(EBL::createHashOfVector(EBL::getScale()) == oldScaleHash);
    }
  }
}

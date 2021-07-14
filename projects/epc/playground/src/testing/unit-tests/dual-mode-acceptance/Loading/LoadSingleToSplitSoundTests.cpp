#include <iostream>
#include <testing/unit-tests/mock/MockPresetStorage.h>
#include <parameters/scale-converters/ParabolicGainDbScaleConverter.h>
#include <testing/unit-tests/mock/PreDualModePresetBank.h>
#include <parameter_declarations.h>
#include "testing/TestHelper.h"
#include "testing/unit-tests/mock/EditBufferNamedLogicalParts.h"
#include "proxies/hwui/HWUI.h"
#include "presets/Preset.h"
#include "presets/PresetParameter.h"
#include "parameters/SplitPointParameter.h"
#include "parameters/scale-converters/ScaleConverter.h"

using EBL = EditBufferLogicalParts;

auto getPresetParameter = [](auto preset, auto parameter) {
  auto id = parameter->getID();
  return preset->findParameterByID(id, false);
};

TEST_CASE("Load Pre 1.7 Preset into Mono Enabled part resets Mono Enable")
{
  auto eb = TestHelper::getEditBuffer();
  PreDualModePresetBank bank;
  auto preset = bank.createMockPreset();

  TestHelper::initDualEditBuffer<SoundType::Split>();

  auto scope = TestHelper::createTestScope();
  auto transaction = scope->getTransaction();

  auto monoEnable = eb->findParameterByID({ C15::PID::Mono_Grp_Enable, VoiceGroup::I });

  monoEnable->setCPFromHwui(transaction, 1);
  CHECK(monoEnable->getDisplayString() == "On");

  eb->undoableLoadToPart(transaction, preset.get(), VoiceGroup::I, VoiceGroup::I);

  CHECK(monoEnable->getDisplayString() == "Off");
}

TEST_CASE("Load Single into Split Part I")
{
  auto eb = TestHelper::getEditBuffer();
  EditBufferUseCases ebUseCases(eb);
  MockPresetStorage presets;
  auto preset = presets.getSinglePreset();

  CHECK(preset->getType() == SoundType::Single);

  {
    auto scope = TestHelper::createTestScope();
    auto transaction = scope->getTransaction();
    TestHelper::initDualEditBuffer<SoundType::Split>(transaction);
    Application::get().getHWUI()->setCurrentVoiceGroup(VoiceGroup::I);

    auto envAAttack = preset->findParameterByID({ 0, VoiceGroup::I }, true);
    envAAttack->setValue(transaction, 0.666);

    auto volume = preset->findParameterByID({ 247, VoiceGroup::Global }, true);
    volume->setValue(transaction, 0.265);
    volume->setField(transaction, PresetParameter::Fields::ModSource, "1");
    volume->setField(transaction, PresetParameter::Fields::ModAmount, "1");

    auto unisonVoices = preset->findParameterByID({ 249, VoiceGroup::I }, true);
    unisonVoices->setValue(transaction, 1);  // <- setting cp in range 0..23

    preset->setName(transaction, "Hi");

    TestHelper::changeAllParameters(transaction);
  }

  WHEN("Load")
  {
    const auto toFXIHash = EBL::createValueHash(EBL::getToFX<VoiceGroup::I>());
    const auto toFXIIHash = EBL::createValueHash(EBL::getToFX<VoiceGroup::II>());
    const auto oldSplitCP
        = eb->findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::I })->getControlPositionValue();
    const auto oldVolumeDisplay = EBL::getMasterVolume()->getDisplayValue();
    const auto oldTuneDisplay = EBL::getMasterTune()->getDisplayValue();
    const auto oldMasterHash = EBL::createHashOfVector(EBL::getMaster());
    const auto oldScaleHash = EBL::createHashOfVector(EBL::getScale());
    const auto oldMCMHash = EBL::createHashOfVector(EBL::getModMatrix());
    const auto oldVoicesIIHash = EBL::createHashOfVector(EBL::getVoices<VoiceGroup::II>());
    const auto oldFadeIIHash = EBL::createHashOfVector(EBL::getFade<VoiceGroup::II>());

    ebUseCases.undoableLoadToPart(preset, VoiceGroup::I, VoiceGroup::I);

    THEN("Type is Same")
    {
      CHECK(eb->getType() == SoundType::Split);
    }

    THEN("Voice Group Label was loaded")
    {
      CHECK(eb->getVoiceGroupName(VoiceGroup::I) == "Hi");
    }

    THEN("toFX unchanged")
    {
      CHECK(EBL::createValueHash(EBL::getToFX<VoiceGroup::I>()) == toFXIHash);
      CHECK(EBL::createValueHash(EBL::getToFX<VoiceGroup::II>()) == toFXIIHash);
    }

    THEN("Cross FB is Default")
    {
      CHECK(EBL::isFactoryDefaultLoaded(EBL::getCrossFB<VoiceGroup::I>()));
      CHECK(EBL::isFactoryDefaultLoaded(EBL::getCrossFB<VoiceGroup::II>()));
    }

    THEN("Local Normal was copied to current VG")
    {
      CHECK_PARAMETER_CP_EQUALS_FICTION(eb->findParameterByID({ 0, VoiceGroup::I }), 0.666);
    }

    THEN("Unison and Mono Loaded from preset")
    {
      CHECK(oldVoicesIIHash == EBL::createHashOfVector(EBL::getVoices<VoiceGroup::II>()));
      CHECK_PARAMETER_CP_EQUALS_FICTION(EBL::getUnisonVoice<VoiceGroup::I>(), 1);  //compare cp for 0..11
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

    THEN("Part Master Modulation correct")
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

    THEN("Fade I is default")
    {
      CHECK(EBL::isFactoryDefaultLoaded(EBL::getFade<VoiceGroup::I>()));
    }

    THEN("Fade II is ignored")
    {
      CHECK(EBL::createHashOfVector(EBL::getFade<VoiceGroup::II>()) == oldFadeIIHash);
    }
  }
}

TEST_CASE("Load Single into Split Part II")
{
  auto eb = TestHelper::getEditBuffer();
  EditBufferUseCases ebUseCases(eb);
  MockPresetStorage presets;
  auto preset = presets.getSinglePreset();

  CHECK(preset->getType() == SoundType::Single);

  {
    auto scope = TestHelper::createTestScope();
    auto transaction = scope->getTransaction();
    TestHelper::initDualEditBuffer<SoundType::Split>(transaction);
    Application::get().getHWUI()->setCurrentVoiceGroup(VoiceGroup::II);

    auto envAAttack = preset->findParameterByID({ 0, VoiceGroup::I }, true);
    envAAttack->setValue(transaction, 0.666);

    auto volume = preset->findParameterByID({ 247, VoiceGroup::Global }, true);
    volume->setValue(transaction, 0.265);
    volume->setField(transaction, PresetParameter::Fields::ModSource, "1");
    volume->setField(transaction, PresetParameter::Fields::ModAmount, "1");

    auto unisonVoices = preset->findParameterByID({ 249, VoiceGroup::I }, true);
    unisonVoices->setValue(transaction, 1);  // <- setting cp in range 0..23

    preset->setName(transaction, "Ho");

    TestHelper::changeAllParameters(transaction);
  }

  WHEN("Load")
  {
    const auto toFXIHash = EBL::createValueHash(EBL::getToFX<VoiceGroup::I>());
    const auto toFXIIHash = EBL::createValueHash(EBL::getToFX<VoiceGroup::II>());
    const auto oldSplitCP
        = eb->findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::I })->getControlPositionValue();
    const auto oldVolumeDisplay = EBL::getMasterVolume()->getDisplayValue();
    const auto oldTuneDisplay = EBL::getMasterTune()->getDisplayValue();
    const auto oldMasterHash = EBL::createHashOfVector(EBL::getMaster());
    const auto oldScaleHash = EBL::createHashOfVector(EBL::getScale());
    const auto oldMCMHash = EBL::createHashOfVector(EBL::getModMatrix());
    const auto oldFadeIHash = EBL::createHashOfVector(EBL::getFade<VoiceGroup::I>());
    const auto oldVoicesIHash = EBL::createHashOfVector(EBL::getVoices<VoiceGroup::I>());

    ebUseCases.undoableLoadToPart(preset, VoiceGroup::I, VoiceGroup::II);

    THEN("Type is Same")
    {
      CHECK(eb->getType() == SoundType::Split);
    }

    THEN("Voice Group Label was loaded")
    {
      CHECK(eb->getVoiceGroupName(VoiceGroup::II) == "Ho");
    }

    THEN("Local Special unchanged")
    {
      CHECK(EBL::createValueHash(EBL::getToFX<VoiceGroup::I>()) == toFXIHash);
      CHECK(EBL::createValueHash(EBL::getToFX<VoiceGroup::II>()) == toFXIIHash);
    }

    THEN("Cross FB is Default")
    {
      CHECK(EBL::isFactoryDefaultLoaded(EBL::getCrossFB<VoiceGroup::I>()));
      CHECK(EBL::isFactoryDefaultLoaded(EBL::getCrossFB<VoiceGroup::II>()));
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

    THEN("Unison and Mono Loaded from preset")
    {
      CHECK(oldVoicesIHash == EBL::createHashOfVector(EBL::getVoices<VoiceGroup::I>()));
      CHECK_PARAMETER_CP_EQUALS_FICTION(EBL::getUnisonVoice<VoiceGroup::II>(), 1);  //compare cp for 0..11
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

    THEN("Part Master Modulation aspects copied")
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

    THEN("Fade I is untouched")
    {
      CHECK(EBL::createHashOfVector(EBL::getFade<VoiceGroup::I>()) == oldFadeIHash);
    }

    THEN("Fade II is Default")
    {
      CHECK(EBL::isFactoryDefaultLoaded(EBL::getFade<VoiceGroup::II>()));
    }
  }
}

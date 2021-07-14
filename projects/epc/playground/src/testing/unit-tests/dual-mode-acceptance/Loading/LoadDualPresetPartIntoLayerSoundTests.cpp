#include <iostream>
#include <testing/unit-tests/mock/MockPresetStorage.h>
#include <parameters/scale-converters/ParabolicGainDbScaleConverter.h>
#include "testing/TestHelper.h"
#include "testing/unit-tests/mock/EditBufferNamedLogicalParts.h"
#include "proxies/hwui/HWUI.h"
#include "presets/Preset.h"
#include "presets/PresetParameter.h"
#include "parameters/SplitPointParameter.h"
#include "parameters/scale-converters/ScaleConverter.h"
#include "proxies/audio-engine/AudioEngineProxy.h"
#include <parameter_declarations.h>

using EBL = EditBufferLogicalParts;

TEST_CASE("Load Part I of Split into Layer Part I")
{
  auto eb = TestHelper::getEditBuffer();
  MockPresetStorage presets;
  auto preset = presets.getSplitPreset();

  CHECK(preset->getType() == SoundType::Split);

  CHECK_NOTHROW(preset->findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::I }, true));

  {
    auto scope = TestHelper::createTestScope();
    auto transaction = scope->getTransaction();
    TestHelper::initDualEditBuffer<SoundType::Layer>(transaction);
    Application::get().getHWUI()->setCurrentVoiceGroup(VoiceGroup::I);

    auto envAAttack = preset->findParameterByID({ 0, VoiceGroup::I }, true);
    envAAttack->setValue(transaction, 0.666);

    auto volume = preset->findParameterByID({ 247, VoiceGroup::Global }, true);
    volume->setValue(transaction, 0.265);

    auto tune = preset->findParameterByID({ 248, VoiceGroup::Global }, true);
    tune->setValue(transaction, 0.265);

    auto partVolume = preset->findParameterByID({ 358, VoiceGroup::I }, true);
    partVolume->setValue(transaction, 0.420);
    partVolume->setField(transaction, PresetParameter::Fields::ModSource, "1");
    partVolume->setField(transaction, PresetParameter::Fields::ModAmount, "1");

    auto unisonVoices = preset->findParameterByID({ 249, VoiceGroup::I }, true);
    unisonVoices->setValue(transaction, 0.25);  // <- setting cp in range 0..23

    for(auto p : EBL::getToFX<VoiceGroup::I>())
    {
      auto toFX = preset->findParameterByID({ p->getID().getNumber(), VoiceGroup::I }, true);
      toFX->setValue(transaction, 0.187);
    }

    TestHelper::changeAllParameters(transaction);
  }

  WHEN("Load")
  {
    const auto oldVolumeDisplay = EBL::getMasterVolume()->getDisplayValue();
    const auto oldTuneDisplay = EBL::getMasterTune()->getDisplayValue();
    const auto oldMasterHash = EBL::createHashOfVector(EBL::getMaster());
    const auto oldScaleHash = EBL::createHashOfVector(EBL::getScale());
    const auto oldMCMHash = EBL::createHashOfVector(EBL::getModMatrix());
    const auto localNormalIIHash = EBL::createValueHash(EBL::getLocalNormal<VoiceGroup::II>());
    const auto oldFadeIHash = EBL::createValueHash(EBL::getFade<VoiceGroup::I>());
    const auto oldFadeIIHash = EBL::createValueHash(EBL::getFade<VoiceGroup::II>());
    const auto oldVoicesIHash = EBL::createValueHash(EBL::getVoices<VoiceGroup::I>());
    const auto oldCrossFBIHash = EBL::createValueHash(EBL::getCrossFB<VoiceGroup::I>());

    auto scope = TestHelper::createTestScope();
    auto transaction = scope->getTransaction();

    auto oldAEMessage = AudioEngineProxy::createLayerEditBufferMessage(*eb);

    eb->undoableLoadToPart(transaction, preset, VoiceGroup::I, VoiceGroup::I);

    THEN("AudioEngine Message is different")
    {
      auto newAEMessage = AudioEngineProxy::createLayerEditBufferMessage(*eb);
      CHECK_FALSE(newAEMessage == oldAEMessage);
    }

    THEN("Type is Same")
    {
      CHECK(eb->getType() == SoundType::Layer);
    }

    THEN("ToFX I copied from Preset")
    {
      for(auto& p : EBL::getToFX<VoiceGroup::I>())
      {
        CHECK_PARAMETER_CP_EQUALS_FICTION(p, 0.187);
      }
    }

    THEN("Cross FB I ignored")
    {
      CHECK(oldCrossFBIHash == EBL::createValueHash(EBL::getCrossFB<VoiceGroup::I>()));
    }

    THEN("Local Normal was copied to current VG")
    {
      CHECK_PARAMETER_CP_EQUALS_FICTION(eb->findParameterByID({ 0, VoiceGroup::I }), 0.666);
    }

    THEN("Local II untouched")
    {
      CHECK(localNormalIIHash == EBL::createValueHash(EBL::getLocalNormal<VoiceGroup::II>()));
    }

    THEN("Unison and Mono II are default")
    {
      CHECK(EBL::isFactoryDefaultLoaded(EBL::getVoices<VoiceGroup::II>()));
    }

    THEN("Unison and Mono of I unchanged")
    {
      CHECK(oldVoicesIHash == EBL::createValueHash(EBL::getVoices<VoiceGroup::I>()));
    }

    THEN("Part Volume and Tune correct")
    {
      auto volumeScale = EBL::getMasterVolume()->getValue().getScaleConverter();
      auto tuneScale = EBL::getMasterTune()->getValue().getScaleConverter();

      PresetParameter* ogMasterVolume = preset->findParameterByID({ 247, VoiceGroup::Global }, true);
      PresetParameter* ogMasterTune = preset->findParameterByID({ 248, VoiceGroup::Global }, true);
      PresetParameter* ogPartTune = preset->findParameterByID({ 360, VoiceGroup::I }, true);
      PresetParameter* ogPartVolume = preset->findParameterByID({ 358, VoiceGroup::I }, true);

      auto presetGlobalVolumeDisplay = volumeScale->controlPositionToDisplay(ogMasterVolume->getValue());
      auto presetPartVolumeDisplay = volumeScale->controlPositionToDisplay(ogPartVolume->getValue());
      const auto newPartVolumeDisplay = (presetGlobalVolumeDisplay + presetPartVolumeDisplay) - oldVolumeDisplay;
      CHECK(EBL::getPartVolume<VoiceGroup::I>()->getDisplayValue() == Approx(newPartVolumeDisplay).epsilon(.005));

      auto presetGlobalTuneDisplay = tuneScale->controlPositionToDisplay(ogMasterTune->getValue());
      auto presetPartTuneDisplay = tuneScale->controlPositionToDisplay(ogPartTune->getValue());
      const auto newPartTuneDisplay
          = std::min((presetGlobalTuneDisplay + presetPartTuneDisplay) - oldTuneDisplay, 48.0);
      CHECK(EBL::getPartTune<VoiceGroup::I>()->getDisplayValue() == Approx(newPartTuneDisplay).epsilon(0.005));
    }

    THEN("Part Volume Modulation assigned")
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

    THEN("Fade I/II untouched")
    {
      CHECK(EBL::createValueHash(EBL::getFade<VoiceGroup::I>()) == oldFadeIHash);
      CHECK(EBL::createValueHash(EBL::getFade<VoiceGroup::II>()) == oldFadeIIHash);
    }
  }
}

TEST_CASE("Load Part I of Split into Layer Part II")
{
  auto eb = TestHelper::getEditBuffer();
  MockPresetStorage presets;
  auto preset = presets.getSplitPreset();

  CHECK(preset->getType() == SoundType::Split);

  {
    auto scope = TestHelper::createTestScope();
    auto transaction = scope->getTransaction();
    TestHelper::initDualEditBuffer<SoundType::Layer>(transaction);
    Application::get().getHWUI()->setCurrentVoiceGroup(VoiceGroup::II);

    auto envAAttack = preset->findParameterByID({ 0, VoiceGroup::I }, true);
    envAAttack->setValue(transaction, 0.666);

    auto volume = preset->findParameterByID({ 247, VoiceGroup::Global }, true);
    volume->setValue(transaction, 0.265);

    auto tune = preset->findParameterByID({ 248, VoiceGroup::Global }, true);
    tune->setValue(transaction, 0.265);

    auto partVolume = preset->findParameterByID({ 358, VoiceGroup::I }, true);
    partVolume->setValue(transaction, 0.420);
    partVolume->setField(transaction, PresetParameter::Fields::ModSource, "1");
    partVolume->setField(transaction, PresetParameter::Fields::ModAmount, "1");

    auto unisonVoices = preset->findParameterByID({ 249, VoiceGroup::I }, true);
    unisonVoices->setValue(transaction, 0.25);  // <- setting cp in range 0..23

    for(auto p : EBL::getToFX<VoiceGroup::I>())
    {
      auto toFX = preset->findParameterByID({ p->getID().getNumber(), VoiceGroup::I }, true);
      toFX->setValue(transaction, 0.187);
    }

    TestHelper::changeAllParameters(transaction);
  }

  WHEN("Load")
  {
    const auto oldSplitCP
        = eb->findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::I })->getControlPositionValue();
    const auto oldVolumeDisplay = EBL::getMasterVolume()->getDisplayValue();
    const auto oldTuneDisplay = EBL::getMasterTune()->getDisplayValue();
    const auto oldMasterHash = EBL::createHashOfVector(EBL::getMaster());
    const auto oldScaleHash = EBL::createHashOfVector(EBL::getScale());
    const auto oldMCMHash = EBL::createHashOfVector(EBL::getModMatrix());
    const auto localNormalIHash = EBL::createValueHash(EBL::getLocalNormal<VoiceGroup::I>());
    const auto oldFadeIHash = EBL::createValueHash(EBL::getFade<VoiceGroup::I>());
    const auto oldFadeIIHash = EBL::createValueHash(EBL::getFade<VoiceGroup::II>());
    const auto oldVoicesIHash = EBL::createValueHash(EBL::getVoices<VoiceGroup::I>());
    const auto oldCrossFBIIHash = EBL::createValueHash(EBL::getCrossFB<VoiceGroup::II>());

    auto scope = TestHelper::createTestScope();
    auto transaction = scope->getTransaction();

    eb->undoableLoadToPart(transaction, preset, VoiceGroup::I, VoiceGroup::II);

    THEN("Type is Same")
    {
      CHECK(eb->getType() == SoundType::Layer);
    }

    THEN("Local Normal was copied to current VG")
    {
      CHECK_PARAMETER_CP_EQUALS_FICTION(eb->findParameterByID({ 0, VoiceGroup::II }), 0.666);
    }

    THEN("ToFX II copied from Preset")
    {
      for(auto& p : EBL::getToFX<VoiceGroup::II>())
      {
        CHECK_PARAMETER_CP_EQUALS_FICTION(p, 0.187);
      }
    }

    THEN("Cross FB II ignored")
    {
      CHECK(oldCrossFBIIHash == EBL::createValueHash(EBL::getCrossFB<VoiceGroup::II>()));
    }

    THEN("Local I untouched")
    {
      CHECK(localNormalIHash == EBL::createValueHash(EBL::getLocalNormal<VoiceGroup::I>()));
    }

    THEN("Split unchanged")
    {
      CHECK_PARAMETER_CP_EQUALS_FICTION(eb->findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::I }),
                                        oldSplitCP);
    }

    THEN("Unison and Mono II are default")
    {
      CHECK(EBL::isFactoryDefaultLoaded(EBL::getVoices<VoiceGroup::II>()));
    }

    THEN("Unison and Mono of I unchanged")
    {
      CHECK(oldVoicesIHash == EBL::createValueHash(EBL::getVoices<VoiceGroup::I>()));
    }

    THEN("Part Volume and Tune correct")
    {
      auto volumeScale = EBL::getMasterVolume()->getValue().getScaleConverter();
      auto tuneScale = EBL::getMasterTune()->getValue().getScaleConverter();

      PresetParameter* vol = preset->findParameterByID({ 247, VoiceGroup::Global }, true);
      PresetParameter* tune = preset->findParameterByID({ 248, VoiceGroup::Global }, true);
      PresetParameter* ogPartTune = preset->findParameterByID({ 360, VoiceGroup::I }, true);
      PresetParameter* ogPartVolume = preset->findParameterByID({ 358, VoiceGroup::I }, true);

      auto presetGlobalVolumeDisplay = volumeScale->controlPositionToDisplay(vol->getValue());
      auto presetPartVolumeDisplay = volumeScale->controlPositionToDisplay(ogPartVolume->getValue());
      const auto newPartVolumeDisplay = (presetGlobalVolumeDisplay + presetPartVolumeDisplay) - oldVolumeDisplay;
      CHECK(EBL::getPartVolume<VoiceGroup::II>()->getDisplayValue() == Approx(newPartVolumeDisplay).epsilon(.005));

      auto presetGlobalTuneDisplay = tuneScale->controlPositionToDisplay(tune->getValue());
      auto presetPartTuneDisplay = tuneScale->controlPositionToDisplay(ogPartTune->getValue());
      const auto newPartTuneDisplay = (presetGlobalTuneDisplay + presetPartTuneDisplay) - oldTuneDisplay;
      CHECK(EBL::getPartTune<VoiceGroup::II>()->getDisplayValue() == Approx(newPartTuneDisplay).epsilon(0.005));
    }

    THEN("Part Volume Modulation assigned")
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

    THEN("Fade I/II untouched")
    {
      CHECK(EBL::createValueHash(EBL::getFade<VoiceGroup::I>()) == oldFadeIHash);
      CHECK(EBL::createValueHash(EBL::getFade<VoiceGroup::II>()) == oldFadeIIHash);
    }
  }
}

TEST_CASE("Load Part I of Layer into Layer Part I")
{
  auto eb = TestHelper::getEditBuffer();
  MockPresetStorage presets;
  auto preset = presets.getLayerPreset();

  REQUIRE(preset->getType() == SoundType::Layer);

  {
    auto scope = TestHelper::createTestScope();
    auto transaction = scope->getTransaction();
    TestHelper::initDualEditBuffer<SoundType::Layer>(transaction);
    Application::get().getHWUI()->setCurrentVoiceGroup(VoiceGroup::I);

    auto envAAttack = preset->findParameterByID({ 0, VoiceGroup::I }, true);
    envAAttack->setValue(transaction, 0.666);

    auto volume = preset->findParameterByID({ 247, VoiceGroup::Global }, true);
    volume->setValue(transaction, 0.265);

    auto tune = preset->findParameterByID({ 248, VoiceGroup::Global }, true);
    tune->setValue(transaction, 0.265);

    auto partVolume = preset->findParameterByID({ 358, VoiceGroup::I }, true);
    partVolume->setValue(transaction, 0.1337);
    partVolume->setField(transaction, PresetParameter::Fields::ModSource, "1");
    partVolume->setField(transaction, PresetParameter::Fields::ModAmount, "1");

    auto unisonVoices = preset->findParameterByID({ 249, VoiceGroup::I }, true);
    unisonVoices->setValue(transaction, 0.5);  // <- setting cp in range 0..23

    auto monoEnable = preset->findParameterByID({ 364, VoiceGroup::I }, true);
    monoEnable->setValue(transaction, 1);

    for(auto p : EBL::getToFX<VoiceGroup::I>())
    {
      auto toFX = preset->findParameterByID({ p->getID().getNumber(), VoiceGroup::I }, true);
      toFX->setValue(transaction, 0.187);
    }

    for(auto p : EBL::getCrossFB<VoiceGroup::I>())
    {
      auto crossFB = preset->findParameterByID({ p->getID().getNumber(), VoiceGroup::I }, true);
      crossFB->setValue(transaction, 0.187);
    }
  }

  WHEN("Load")
  {
    const auto oldSplitCP
        = eb->findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::I })->getControlPositionValue();
    const auto oldVolumeDisplay = EBL::getMasterVolume()->getDisplayValue();
    const auto oldTuneDisplay = EBL::getMasterTune()->getDisplayValue();
    const auto oldMasterHash = EBL::createHashOfVector(EBL::getMaster());
    const auto oldScaleHash = EBL::createHashOfVector(EBL::getScale());
    const auto oldMCMHash = EBL::createHashOfVector(EBL::getModMatrix());
    const auto localNormalIIHash = EBL::createValueHash(EBL::getLocalNormal<VoiceGroup::II>());
    const auto oldVoicesIHash = EBL::createValueHash(EBL::getVoices<VoiceGroup::I>());
    const auto oldFadeIHash = EBL::createHashOfVector(EBL::getFade<VoiceGroup::I>());
    const auto oldFadeIIHash = EBL::createHashOfVector(EBL::getFade<VoiceGroup::II>());

    auto scope = TestHelper::createTestScope();
    auto transaction = scope->getTransaction();

    eb->undoableLoadToPart(transaction, preset, VoiceGroup::I, VoiceGroup::I);

    THEN("Type is Same")
    {
      CHECK(eb->getType() == SoundType::Layer);
    }

    THEN("ToFX I copied from Preset")
    {
      for(auto& p : EBL::getToFX<VoiceGroup::I>())
      {
        CHECK_PARAMETER_CP_EQUALS_FICTION(p, 0.187);
      }
    }

    THEN("Cross FB I copied from Preset")
    {
      for(auto& p : EBL::getCrossFB<VoiceGroup::I>())
      {
        CHECK_PARAMETER_CP_EQUALS_FICTION(p, 0.187);
      }
    }

    THEN("Local Normal was copied to current VG")
    {
      CHECK_PARAMETER_CP_EQUALS_FICTION(eb->findParameterByID({ 0, VoiceGroup::I }), 0.666);
    }

    THEN("Local II untouched")
    {
      CHECK(localNormalIIHash == EBL::createValueHash(EBL::getLocalNormal<VoiceGroup::II>()));
    }

    THEN("Split unchanged")
    {
      CHECK_PARAMETER_CP_EQUALS_FICTION(eb->findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::I }),
                                        oldSplitCP);
    }

    THEN("Unison and Mono II are default")
    {
      CHECK(EBL::isFactoryDefaultLoaded(EBL::getVoices<VoiceGroup::II>()));
    }

    THEN("Unison and Mono of I unchanged")
    {
      CHECK(oldVoicesIHash == EBL::createValueHash(EBL::getVoices<VoiceGroup::I>()));
    }

    THEN("Part Volume and Tune correct")
    {
      auto volumeScale = EBL::getMasterVolume()->getValue().getScaleConverter();
      auto tuneScale = EBL::getMasterTune()->getValue().getScaleConverter();

      PresetParameter* vol = preset->findParameterByID({ 247, VoiceGroup::Global }, true);
      PresetParameter* tune = preset->findParameterByID({ 248, VoiceGroup::Global }, true);
      PresetParameter* ogPartTune = preset->findParameterByID({ 360, VoiceGroup::I }, true);
      PresetParameter* ogPartVolume = preset->findParameterByID({ 358, VoiceGroup::I }, true);

      const auto presetGlobalVolumeDisplay = volumeScale->controlPositionToDisplay(vol->getValue());
      const auto presetPartVolumeDisplay = volumeScale->controlPositionToDisplay(ogPartVolume->getValue());
      const auto newPartVolumeDisplay = (presetGlobalVolumeDisplay + presetPartVolumeDisplay) - oldVolumeDisplay;
      CHECK(EBL::getPartVolume<VoiceGroup::I>()->getDisplayValue() == Approx(newPartVolumeDisplay).epsilon(0.005));

      const auto presetGlobalTuneDisplay = tuneScale->controlPositionToDisplay(tune->getValue());
      const auto presetPartTuneDisplay = tuneScale->controlPositionToDisplay(ogPartTune->getValue());
      const auto newPartTuneDisplay
          = std::min((presetGlobalTuneDisplay + presetPartTuneDisplay) - oldTuneDisplay, 48.0);
      CHECK(EBL::getPartTune<VoiceGroup::I>()->getDisplayValue() == Approx(newPartTuneDisplay).epsilon(0.005));
    }

    THEN("Part Master Modulation has been copied")
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

    THEN("Fade I/II ignored")
    {
      CHECK(oldFadeIHash == EBL::createHashOfVector(EBL::getFade<VoiceGroup::I>()));
      CHECK(oldFadeIIHash == EBL::createHashOfVector(EBL::getFade<VoiceGroup::II>()));
    }
  }
}

TEST_CASE("Load Part I of Layer into Layer Part II")
{
  auto eb = TestHelper::getEditBuffer();
  MockPresetStorage presets;
  auto preset = presets.getLayerPreset();

  REQUIRE(preset->getType() == SoundType::Layer);

  {
    auto scope = TestHelper::createTestScope();
    auto transaction = scope->getTransaction();
    TestHelper::initDualEditBuffer<SoundType::Layer>(transaction);
    Application::get().getHWUI()->setCurrentVoiceGroup(VoiceGroup::II);

    auto envAAttack = preset->findParameterByID({ 0, VoiceGroup::I }, true);
    envAAttack->setValue(transaction, 0.666);

    auto volume = preset->findParameterByID({ 247, VoiceGroup::Global }, true);
    volume->setValue(transaction, 0.265);

    auto tune = preset->findParameterByID({ 248, VoiceGroup::Global }, true);
    tune->setValue(transaction, 0.265);

    auto partVolume = preset->findParameterByID({ 358, VoiceGroup::I }, true);
    partVolume->setValue(transaction, 0.1337);
    partVolume->setField(transaction, PresetParameter::Fields::ModSource, "1");
    partVolume->setField(transaction, PresetParameter::Fields::ModAmount, "1");

    auto unisonVoices = preset->findParameterByID({ 249, VoiceGroup::I }, true);
    unisonVoices->setValue(transaction, 0.5);  // <- setting cp in range 0..23

    auto monoEnable = preset->findParameterByID({ 364, VoiceGroup::I }, true);
    monoEnable->setValue(transaction, 1);

    for(auto p : EBL::getToFX<VoiceGroup::I>())
    {
      auto toFX = preset->findParameterByID({ p->getID().getNumber(), VoiceGroup::I }, true);
      toFX->setValue(transaction, 0.187);
    }

    for(auto p : EBL::getCrossFB<VoiceGroup::I>())
    {
      auto crossFB = preset->findParameterByID({ p->getID().getNumber(), VoiceGroup::I }, true);
      crossFB->setValue(transaction, 0.187);
    }
  }

  WHEN("Load")
  {
    const auto oldSplitCP
        = eb->findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::I })->getControlPositionValue();
    const auto oldVolumeDisplay = EBL::getMasterVolume()->getDisplayValue();
    const auto oldTuneDisplay = EBL::getMasterTune()->getDisplayValue();
    const auto oldMasterHash = EBL::createHashOfVector(EBL::getMaster());
    const auto oldScaleHash = EBL::createHashOfVector(EBL::getScale());
    const auto oldMCMHash = EBL::createHashOfVector(EBL::getModMatrix());
    const auto localNormalIHash = EBL::createValueHash(EBL::getLocalNormal<VoiceGroup::I>());
    const auto oldVoicesIHash = EBL::createHashOfVector(EBL::getVoices<VoiceGroup::I>());
    const auto oldFadeIHash = EBL::createHashOfVector(EBL::getFade<VoiceGroup::I>());
    const auto oldFadeIIHash = EBL::createHashOfVector(EBL::getFade<VoiceGroup::II>());

    auto scope = TestHelper::createTestScope();
    auto transaction = scope->getTransaction();

    eb->undoableLoadToPart(transaction, preset, VoiceGroup::I, VoiceGroup::II);

    THEN("Type is Same")
    {
      CHECK(eb->getType() == SoundType::Layer);
    }

    THEN("ToFX II copied from Preset")
    {
      for(auto& p : EBL::getToFX<VoiceGroup::II>())
      {
        CHECK_PARAMETER_CP_EQUALS_FICTION(p, 0.187);
      }
    }

    THEN("Cross FB II copied from Preset")
    {
      for(auto& p : EBL::getCrossFB<VoiceGroup::II>())
      {
        CHECK_PARAMETER_CP_EQUALS_FICTION(p, 0.187);
      }
    }

    THEN("Local Normal was copied to current VG")
    {
      CHECK_PARAMETER_CP_EQUALS_FICTION(eb->findParameterByID({ 0, VoiceGroup::II }), 0.666);
    }

    THEN("Local I untouched")
    {
      CHECK(localNormalIHash == EBL::createValueHash(EBL::getLocalNormal<VoiceGroup::I>()));
    }

    THEN("Split unchanged")
    {
      CHECK_PARAMETER_CP_EQUALS_FICTION(eb->findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::I }),
                                        oldSplitCP);
    }

    THEN("Unison and Mono II are default")
    {
      CHECK(
          EBL::isFactoryDefaultLoaded({ EBL::getUnisonVoice<VoiceGroup::II>(), EBL::getMonoEnable<VoiceGroup::II>() }));
    }

    THEN("Unison and Mono of I unchanged")
    {
      CHECK(oldVoicesIHash == EBL::createHashOfVector(EBL::getVoices<VoiceGroup::I>()));
    }

    THEN("Part Volume and Tune correct")
    {
      auto volumeScale = EBL::getMasterVolume()->getValue().getScaleConverter();
      auto tuneScale = EBL::getMasterTune()->getValue().getScaleConverter();

      PresetParameter* vol = preset->findParameterByID({ 247, VoiceGroup::Global }, true);
      PresetParameter* tune = preset->findParameterByID({ 248, VoiceGroup::Global }, true);
      PresetParameter* ogPartTune = preset->findParameterByID({ 360, VoiceGroup::I }, true);
      PresetParameter* ogPartVolume = preset->findParameterByID({ 358, VoiceGroup::I }, true);

      const auto presetGlobalVolumeDisplay = volumeScale->controlPositionToDisplay(vol->getValue());
      const auto presetPartVolumeDisplay = volumeScale->controlPositionToDisplay(ogPartVolume->getValue());
      const auto newPartVolumeDisplay = (presetGlobalVolumeDisplay + presetPartVolumeDisplay) - oldVolumeDisplay;
      CHECK(EBL::getPartVolume<VoiceGroup::II>()->getDisplayValue() == Approx(newPartVolumeDisplay).epsilon(0.005));

      const auto presetGlobalTuneDisplay = tuneScale->controlPositionToDisplay(tune->getValue());
      const auto presetPartTuneDisplay = tuneScale->controlPositionToDisplay(ogPartTune->getValue());
      const auto newPartTuneDisplay = (presetGlobalTuneDisplay + presetPartTuneDisplay) - oldTuneDisplay;
      CHECK(EBL::getPartTune<VoiceGroup::II>()->getDisplayValue() == Approx(newPartTuneDisplay));
    }

    THEN("Part Master Modulation was copied")
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

    THEN("Fade I/II ignored")
    {
      CHECK(oldFadeIHash == EBL::createHashOfVector(EBL::getFade<VoiceGroup::I>()));
      CHECK(oldFadeIIHash == EBL::createHashOfVector(EBL::getFade<VoiceGroup::II>()));
    }
  }
}

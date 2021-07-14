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
#include <parameter_declarations.h>

using EBL = EditBufferLogicalParts;

TEST_CASE("Load Part I of Split into Split Part I")
{
  auto eb = TestHelper::getEditBuffer();
  MockPresetStorage presets;
  auto preset = presets.getSplitPreset();

  CHECK(preset->getType() == SoundType::Split);

  {
    auto scope = TestHelper::createTestScope();
    auto transaction = scope->getTransaction();
    TestHelper::initDualEditBuffer<SoundType::Split>(transaction);
    Application::get().getHWUI()->setCurrentVoiceGroup(VoiceGroup::I);

    auto envAAttack = preset->findParameterByID({ 0, VoiceGroup::I }, true);
    envAAttack->setValue(transaction, 0.666);

    auto volume = preset->findParameterByID({ 247, VoiceGroup::Global }, true);
    volume->setValue(transaction, 0.265);

    auto tune = preset->findParameterByID({ 248, VoiceGroup::Global }, true);
    tune->setValue(transaction, 0.265);

    auto partVolume = preset->findParameterByID({ 358, VoiceGroup::I }, true);
    partVolume->setValue(transaction, 0.5);
    partVolume->setField(transaction, PresetParameter::Fields::ModSource, "1");
    partVolume->setField(transaction, PresetParameter::Fields::ModAmount, "1");

    auto unisonVoices = preset->findParameterByID({ 249, VoiceGroup::I }, true);
    unisonVoices->setValue(transaction, 0.25);  // <- setting cp in range 0..23

    auto ebSplitI = eb->findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::I });
    auto presetSplitI = preset->findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::II }, false);

    ebSplitI->setCPFromHwui(transaction, 0.2);
    presetSplitI->setValue(transaction, 1);

    for(auto& p : EBL::getToFX<VoiceGroup::I>())
    {
      if(auto toFxP = preset->findParameterByID(p->getID(), false))
      {
        toFxP->setValue(transaction, 0.7);
      }
      else
      {
        CHECK(false);
      }
    }

    TestHelper::changeAllParameters(transaction);
  }

  WHEN("Load")
  {
    const auto toFXIIHash = EBL::createValueHash(EBL::getToFX<VoiceGroup::II>());
    const auto oldVolumeDisplay = EBL::getMasterVolume()->getDisplayValue();
    const auto oldTuneDisplay = EBL::getMasterTune()->getDisplayValue();
    const auto oldMasterHash = EBL::createHashOfVector(EBL::getMaster());
    const auto oldScaleHash = EBL::createHashOfVector(EBL::getScale());
    const auto oldMCMHash = EBL::createHashOfVector(EBL::getModMatrix());
    const auto localNormalIIHash = EBL::createValueHash(EBL::getLocalNormal<VoiceGroup::II>());
    const auto oldFadeIIHash = EBL::createHashOfVector(EBL::getFade<VoiceGroup::II>());
    const auto oldVoicesIIHash = EBL::createHashOfVector(EBL::getVoices<VoiceGroup::II>());

    auto scope = TestHelper::createTestScope();
    auto transaction = scope->getTransaction();

    const auto oldSplitPos
        = eb->findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::I })->getControlPositionValue();

    eb->undoableLoadToPart(transaction, preset, VoiceGroup::I, VoiceGroup::I);

    THEN("Type is Same")
    {
      CHECK(eb->getType() == SoundType::Split);
    }

    THEN("Split was ignored")
    {
      CHECK_PARAMETER_CP_EQUALS_FICTION(eb->findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::I }),
                                        oldSplitPos);
    }

    THEN("toFX and local normal II untouched")
    {
      CHECK(localNormalIIHash == EBL::createValueHash(EBL::getLocalNormal<VoiceGroup::II>()));
      CHECK(toFXIIHash == EBL::createValueHash(EBL::getToFX<VoiceGroup::II>()));
    }

    THEN("ToFX I copied / Cross FB I default")
    {
      CHECK(EBL::isFactoryDefaultLoaded(EBL::getCrossFB<VoiceGroup::I>()));

      for(auto& p : EBL::getToFX<VoiceGroup::I>())
        CHECK_PARAMETER_CP_EQUALS_FICTION(p, 0.7);
    }

    THEN("Cross FB II is Default")
    {
      CHECK(EBL::isFactoryDefaultLoaded(EBL::getCrossFB<VoiceGroup::II>()));
    }

    THEN("Local Normal was copied to current VG")
    {
      CHECK_PARAMETER_CP_EQUALS_FICTION(eb->findParameterByID({ 0, VoiceGroup::I }), 0.666);
    }

    THEN("Unison and Mono of I Copied to I")
    {
      CHECK_PARAMETER_CP_EQUALS_FICTION(EBL::getUnisonVoice<VoiceGroup::I>(), 0.5);  //compare cp for 0..11
    }

    THEN("Unison and Mono of II unchanged")
    {
      CHECK(oldVoicesIIHash == EBL::createHashOfVector(EBL::getVoices<VoiceGroup::II>()));
    }

    THEN("Fade II untouched")
    {
      CHECK(EBL::createHashOfVector(EBL::getFade<VoiceGroup::II>()) == oldFadeIIHash);
    }

    THEN("Fade I Default")
    {
      CHECK(EBL::isFactoryDefaultLoaded(EBL::getFade<VoiceGroup::I>()));
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

    THEN("Part Modulation Copied")
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

TEST_CASE("Load Part I of Split into Split Part II")
{
  auto eb = TestHelper::getEditBuffer();
  MockPresetStorage presets;
  auto preset = presets.getSplitPreset();

  CHECK(preset->getType() == SoundType::Split);

  {
    auto scope = TestHelper::createTestScope();
    auto transaction = scope->getTransaction();
    TestHelper::initDualEditBuffer<SoundType::Split>(transaction);
    Application::get().getHWUI()->setCurrentVoiceGroup(VoiceGroup::II);

    auto envAAttack = preset->findParameterByID({ 0, VoiceGroup::I }, true);
    envAAttack->setValue(transaction, 0.666);

    auto volume = preset->findParameterByID({ 247, VoiceGroup::Global }, true);
    volume->setValue(transaction, 0.265);

    auto tune = preset->findParameterByID({ 248, VoiceGroup::Global }, true);
    tune->setValue(transaction, 0.265);

    auto pSplit = preset->findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::I }, false);
    pSplit->setValue(transaction, 1);

    auto eSplit = eb->findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::I });
    eSplit->setCPFromHwui(transaction, 0);

    auto partVolume = preset->findParameterByID({ 358, VoiceGroup::I }, true);
    partVolume->setField(transaction, PresetParameter::Fields::ModSource, "1");
    partVolume->setField(transaction, PresetParameter::Fields::ModAmount, "1");

    auto unisonVoices = preset->findParameterByID({ 249, VoiceGroup::I }, true);
    unisonVoices->setValue(transaction, 0.25);  // <- setting cp in range 0..23

    for(auto& p : EBL::getToFX<VoiceGroup::I>())
    {
      if(auto toFxP = preset->findParameterByID(p->getID(), false))
      {
        toFxP->setValue(transaction, 0.7);
      }
      else
      {
        CHECK(false);
      }
    }

    TestHelper::changeAllParameters(transaction);
  }

  WHEN("Load")
  {
    const auto toFXIHash = EBL::createValueHash(EBL::getToFX<VoiceGroup::I>());
    const auto oldSplitCP
        = eb->findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::I })->getControlPositionValue();
    const auto oldVolumeDisplay = EBL::getMasterVolume()->getDisplayValue();
    const auto oldTuneDisplay = EBL::getMasterTune()->getDisplayValue();
    const auto oldMasterHash = EBL::createHashOfVector(EBL::getMaster());
    const auto oldScaleHash = EBL::createHashOfVector(EBL::getScale());
    const auto oldMCMHash = EBL::createHashOfVector(EBL::getModMatrix());
    const auto localNormalIHash = EBL::createValueHash(EBL::getLocalNormal<VoiceGroup::I>());
    const auto oldFadeIHash = EBL::createHashOfVector(EBL::getFade<VoiceGroup::I>());
    const auto oldVoicesIHash = EBL::createHashOfVector(EBL::getVoices<VoiceGroup::I>());

    auto scope = TestHelper::createTestScope();
    auto transaction = scope->getTransaction();

    eb->undoableLoadToPart(transaction, preset, VoiceGroup::I, VoiceGroup::II);

    THEN("Type is Same")
    {
      CHECK(eb->getType() == SoundType::Split);
    }

    THEN("Local I untouched")
    {
      CHECK(localNormalIHash == EBL::createValueHash(EBL::getLocalNormal<VoiceGroup::I>()));
    }

    THEN("Cross FB I is default")
    {
      CHECK(EBL::isFactoryDefaultLoaded(EBL::getCrossFB<VoiceGroup::I>()));
    }

    THEN("toFX I unchanged")
    {
      CHECK(EBL::createValueHash(EBL::getToFX<VoiceGroup::I>()) == toFXIHash);
    }

    THEN("ToFX II copied / Cross FB II default")
    {
      CHECK(EBL::isFactoryDefaultLoaded(EBL::getCrossFB<VoiceGroup::II>()));

      for(auto& p : EBL::getToFX<VoiceGroup::II>())
        CHECK_PARAMETER_CP_EQUALS_FICTION(p, 0.7);
    }

    THEN("Local Normal was copied to current VG")
    {
      CHECK_PARAMETER_CP_EQUALS_FICTION(eb->findParameterByID({ 0, VoiceGroup::II }), 0.666);
    }

    THEN("Split unchanged / not loaded")
    {
      CHECK_PARAMETER_CP_EQUALS_FICTION(eb->findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::I }),
                                        oldSplitCP);
    }

    THEN("Unison and Mono of I Copied to II")
    {
      CHECK_PARAMETER_CP_EQUALS_FICTION(EBL::getUnisonVoice<VoiceGroup::II>(), 0.5);  //compare cp for 0..11
    }

    THEN("Unison and Mono of I unchanged")
    {
      CHECK(oldVoicesIHash == EBL::createHashOfVector(EBL::getVoices<VoiceGroup::I>()));
    }

    THEN("Fade I untouched")
    {
      CHECK(EBL::createHashOfVector(EBL::getFade<VoiceGroup::I>()) == oldFadeIHash);
    }

    THEN("Fade II Default")
    {
      CHECK(EBL::isFactoryDefaultLoaded(EBL::getFade<VoiceGroup::II>()));
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
      CHECK(EBL::getPartTune<VoiceGroup::II>()->getDisplayValue() == Approx(newPartTuneDisplay).epsilon(0.005));
    }

    THEN("Part Master Modulation correct")
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

TEST_CASE("Load Part I of Layer into Split Part I")
{
  auto eb = TestHelper::getEditBuffer();
  MockPresetStorage presets;
  auto preset = presets.getLayerPreset();

  REQUIRE(preset->getType() == SoundType::Layer);

  {
    auto scope = TestHelper::createTestScope();
    auto transaction = scope->getTransaction();
    TestHelper::initDualEditBuffer<SoundType::Split>(transaction);
    Application::get().getHWUI()->setCurrentVoiceGroup(VoiceGroup::II);

    auto envAAttack = preset->findParameterByID({ 0, VoiceGroup::I }, true);
    envAAttack->setValue(transaction, 0.666);

    auto volume = preset->findParameterByID({ 247, VoiceGroup::Global }, true);
    volume->setValue(transaction, 0.265);

    auto tune = preset->findParameterByID({ 248, VoiceGroup::Global }, true);
    tune->setValue(transaction, 0.265);

    auto partVolume = preset->findParameterByID({ 358, VoiceGroup::I }, true);
    partVolume->setValue(transaction, 0.187);
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

    TestHelper::changeAllParameters(transaction);
  }

  WHEN("Load")
  {
    const auto oldToFXIIHash = EBL::createValueHash(EBL::getToFX<VoiceGroup::II>());
    const auto oldSplitCP
        = eb->findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::I })->getControlPositionValue();
    const auto oldVolumeDisplay = EBL::getMasterVolume()->getDisplayValue();
    const auto oldTuneDisplay = EBL::getMasterTune()->getDisplayValue();
    const auto oldMasterHash = EBL::createHashOfVector(EBL::getMaster());
    const auto oldScaleHash = EBL::createHashOfVector(EBL::getScale());
    const auto oldMCMHash = EBL::createHashOfVector(EBL::getModMatrix());
    const auto localNormalIIHash = EBL::createValueHash(EBL::getLocalNormal<VoiceGroup::II>());
    const auto oldVoicesIHash = EBL::createHashOfVector(EBL::getVoices<VoiceGroup::II>());

    auto scope = TestHelper::createTestScope();
    auto transaction = scope->getTransaction();

    eb->undoableLoadToPart(transaction, preset, VoiceGroup::I, VoiceGroup::I);

    THEN("Type is Same")
    {
      CHECK(eb->getType() == SoundType::Split);
    }

    THEN("Cross FB is default")
    {
      CHECK(EBL::isFactoryDefaultLoaded(EBL::getCrossFB<VoiceGroup::I>()));
      CHECK(EBL::isFactoryDefaultLoaded(EBL::getCrossFB<VoiceGroup::II>()));
    }

    THEN("toFX and local II unchanged")
    {
      CHECK(localNormalIIHash == EBL::createValueHash(EBL::getLocalNormal<VoiceGroup::II>()));
      CHECK(EBL::createValueHash(EBL::getToFX<VoiceGroup::II>()) == oldToFXIIHash);
    }

    THEN("Local Normal was copied to current VG")
    {
      CHECK_PARAMETER_CP_EQUALS_FICTION(eb->findParameterByID({ 0, VoiceGroup::I }), 0.666);
    }

    THEN("ToFX I copied from Preset")
    {
      for(auto& p : EBL::getToFX<VoiceGroup::I>())
      {
        CHECK_PARAMETER_CP_EQUALS_FICTION(p, 0.187);
      }
    }

    THEN("Fade default")
    {
      CHECK(EBL::isFactoryDefaultLoaded(EBL::getFade<VoiceGroup::I>()));
      CHECK(EBL::isFactoryDefaultLoaded(EBL::getFade<VoiceGroup::II>()));
    }

    THEN("Unison and Mono I are copied From Unison/Mono I of Preset")
    {
      CHECK(EBL::getUnisonVoice<VoiceGroup::I>()->getDisplayString() == "12 voices");
      CHECK(EBL::getMonoEnable<VoiceGroup::I>()->getDisplayString() == "On");
    }

    THEN("Unison and Mono of II unchanged")
    {
      CHECK(oldVoicesIHash == EBL::createHashOfVector(EBL::getVoices<VoiceGroup::II>()));
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
      CHECK(EBL::getPartTune<VoiceGroup::I>()->getDisplayValue() == Approx(newPartTuneDisplay));
    }

    THEN("Part Master Modulation Aspects copied")
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

TEST_CASE("Load Part II of Layer into Split Part II")
{
  auto eb = TestHelper::getEditBuffer();
  MockPresetStorage presets;
  auto preset = presets.getLayerPreset();

  REQUIRE(preset->getType() == SoundType::Layer);

  {
    auto scope = TestHelper::createTestScope();
    auto transaction = scope->getTransaction();
    TestHelper::initDualEditBuffer<SoundType::Split>(transaction);

    auto envAAttack = preset->findParameterByID({ 0, VoiceGroup::II }, true);
    envAAttack->setValue(transaction, 0.666);

    auto volume = preset->findParameterByID({ 247, VoiceGroup::Global }, true);
    volume->setValue(transaction, 0.265);

    auto tune = preset->findParameterByID({ 248, VoiceGroup::Global }, true);
    tune->setValue(transaction, 0.265);

    auto partVolume = preset->findParameterByID({ 358, VoiceGroup::II }, true);
    partVolume->setField(transaction, PresetParameter::Fields::ModSource, "1");
    partVolume->setField(transaction, PresetParameter::Fields::ModAmount, "1");

    auto unisonVoices = preset->findParameterByID({ 249, VoiceGroup::I }, true);
    unisonVoices->setValue(transaction, 0.5);  // <- setting cp in range 0..23

    auto monoEnable = preset->findParameterByID({ 364, VoiceGroup::I }, true);
    monoEnable->setValue(transaction, 1);

    for(auto p : EBL::getToFX<VoiceGroup::II>())
    {
      auto toFX = preset->findParameterByID(p->getID(), true);
      toFX->setValue(transaction, 0.187);
    }

    TestHelper::changeAllParameters(transaction);
  }

  WHEN("Load")
  {
    const auto toFXIHash = EBL::createValueHash(EBL::getToFX<VoiceGroup::I>());
    const auto oldSplitCP
        = eb->findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::I })->getControlPositionValue();
    const auto oldVolumeDisplay = EBL::getMasterVolume()->getDisplayValue();
    const auto oldTuneDisplay = EBL::getMasterTune()->getDisplayValue();
    const auto oldMasterHash = EBL::createHashOfVector(EBL::getMaster());
    const auto oldScaleHash = EBL::createHashOfVector(EBL::getScale());
    const auto oldMCMHash = EBL::createHashOfVector(EBL::getModMatrix());
    const auto localNormalIHash = EBL::createValueHash(EBL::getLocalNormal<VoiceGroup::I>());
    const auto oldVoicesIHash = EBL::createHashOfVector(EBL::getVoices<VoiceGroup::I>());

    auto scope = TestHelper::createTestScope();
    auto transaction = scope->getTransaction();

    eb->undoableLoadToPart(transaction, preset, VoiceGroup::II, VoiceGroup::II);

    THEN("Type is Same")
    {
      CHECK(eb->getType() == SoundType::Split);
    }

    THEN("toFX and local I unchanged")
    {
      CHECK(EBL::createValueHash(EBL::getToFX<VoiceGroup::I>()) == toFXIHash);
      CHECK(localNormalIHash == EBL::createValueHash(EBL::getLocalNormal<VoiceGroup::I>()));
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

    THEN("Cross FB default")
    {
      CHECK(EBL::isFactoryDefaultLoaded(EBL::getCrossFB<VoiceGroup::I>()));
      CHECK(EBL::isFactoryDefaultLoaded(EBL::getCrossFB<VoiceGroup::II>()));
    }

    THEN("Fade II default")
    {
      CHECK(EBL::isFactoryDefaultLoaded(EBL::getFade<VoiceGroup::II>()));
    }

    THEN("Split unchanged")
    {
      CHECK_PARAMETER_CP_EQUALS_FICTION(eb->findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::I }),
                                        oldSplitCP);
    }

    THEN("Unison and Mono II are copied From Unison/Mono I of Preset")
    {
      CHECK(EBL::getUnisonVoice<VoiceGroup::II>()->getDisplayString() == "12 voices");
      CHECK(EBL::getMonoEnable<VoiceGroup::II>()->getDisplayString() == "On");
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
      PresetParameter* ogPartTune = preset->findParameterByID({ 360, VoiceGroup::II }, true);
      PresetParameter* ogPartVolume = preset->findParameterByID({ 358, VoiceGroup::II }, true);

      const auto presetGlobalVolumeDisplay = volumeScale->controlPositionToDisplay(vol->getValue());
      const auto presetPartVolumeDisplay = volumeScale->controlPositionToDisplay(ogPartVolume->getValue());
      const auto newPartVolumeDisplay = (presetGlobalVolumeDisplay + presetPartVolumeDisplay) - oldVolumeDisplay;
      CHECK(EBL::getPartVolume<VoiceGroup::II>()->getDisplayValue() == Approx(newPartVolumeDisplay).epsilon(0.005));

      const auto presetGlobalTuneDisplay = tuneScale->controlPositionToDisplay(tune->getValue());
      const auto presetPartTuneDisplay = tuneScale->controlPositionToDisplay(ogPartTune->getValue());
      const auto newPartTuneDisplay = (presetGlobalTuneDisplay + presetPartTuneDisplay) - oldTuneDisplay;
      CHECK(EBL::getPartTune<VoiceGroup::II>()->getDisplayValue() == Approx(newPartTuneDisplay).epsilon(0.005));
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

TEST_CASE("Split EditBuffer With Split Modulation Load into Part of already Modulated Split")
{
  MockPresetStorage presets;

  TestHelper::initDualEditBuffer<SoundType::Split>();
  auto eb = TestHelper::getEditBuffer();

  auto sI = eb->findAndCastParameterByID<SplitPointParameter>({ C15::PID::Split_Split_Point, VoiceGroup::I });
  auto sII = eb->findAndCastParameterByID<SplitPointParameter>({ C15::PID::Split_Split_Point, VoiceGroup::II });

  {
    CHECK(sI->getModulationSource() == MacroControls::NONE);
    CHECK(sII->getModulationSource() == MacroControls::NONE);

    ModParameterUseCases useCase(sI);
    useCase.selectModSource(MacroControls::MC1);
    useCase.setModulationAmount(50);

    CHECK(sI->getModulationSource() == sII->getModulationSource());
    CHECK(sI->getModulationAmount() == sII->getModulationAmount());
    CHECK(sI->getModulationSource() == MacroControls::MC1);
    CHECK(sII->getModulationSource() == MacroControls::MC1);
  }

  auto preset = presets.getSplitPreset();

  {
    auto psI = preset->findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::I }, false);
    auto psII = preset->findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::II }, false);
    CHECK(psI->getModulationSource() == MacroControls::NONE);
    CHECK(psII->getModulationSource() == MacroControls::NONE);
    CHECK(psI->getModulationAmount() == 0);
    CHECK(psII->getModulationAmount() == 0);
  }

  EditBufferUseCases useCases(eb);

  WHEN("Default split loaded into part of modulated split I -> II")
  {
    useCases.undoableLoadToPart(preset, VoiceGroup::I, VoiceGroup::II);

    CHECK(sI->getModulationSource() == sII->getModulationSource());
    CHECK(sI->getModulationAmount() == sII->getModulationAmount());
  }

  WHEN("Default split loaded into part of modulated split II -> I")
  {
    useCases.undoableLoadToPart(preset, VoiceGroup::II, VoiceGroup::I);

    CHECK(sI->getModulationSource() == sII->getModulationSource());
    CHECK(sI->getModulationAmount() == sII->getModulationAmount());
  }

  WHEN("Default split loaded into part of modulated split I -> I")
  {
    useCases.undoableLoadToPart(preset, VoiceGroup::I, VoiceGroup::I);

    CHECK(sI->getModulationSource() == sII->getModulationSource());
    CHECK(sI->getModulationAmount() == sII->getModulationAmount());
  }

  WHEN("Default split loaded into part of modulated split II -> II")
  {
    useCases.undoableLoadToPart(preset, VoiceGroup::II, VoiceGroup::II);

    CHECK(sI->getModulationSource() == sII->getModulationSource());
    CHECK(sI->getModulationAmount() == sII->getModulationAmount());
  }
}

TEST_CASE("Single EditBuffer Load into Part of already Modulated Split")
{
  MockPresetStorage presets;

  TestHelper::initDualEditBuffer<SoundType::Split>();
  auto eb = TestHelper::getEditBuffer();

  auto sI = eb->findAndCastParameterByID<SplitPointParameter>({ C15::PID::Split_Split_Point, VoiceGroup::I });
  auto sII = eb->findAndCastParameterByID<SplitPointParameter>({ C15::PID::Split_Split_Point, VoiceGroup::II });

  {
    CHECK(sI->getModulationSource() == MacroControls::NONE);
    CHECK(sII->getModulationSource() == MacroControls::NONE);

    ModParameterUseCases useCase(sI);
    useCase.selectModSource(MacroControls::MC1);
    useCase.setModulationAmount(50);

    CHECK(sI->getModulationSource() == sII->getModulationSource());
    CHECK(sI->getModulationAmount() == sII->getModulationAmount());
    CHECK(sI->getModulationSource() == MacroControls::MC1);
    CHECK(sII->getModulationSource() == MacroControls::MC1);
  }

  //TODO LOAD default split and check for inconsistencys
  auto preset = presets.getSinglePreset();

  {
    auto psI = preset->findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::I }, false);
    auto psII = preset->findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::II }, false);
    CHECK(psI->getModulationSource() == MacroControls::NONE);
    CHECK(psII->getModulationSource() == MacroControls::NONE);
    CHECK(psI->getModulationAmount() == 0);
    CHECK(psII->getModulationAmount() == 0);
  }

  EditBufferUseCases useCases(eb);

  WHEN("Default split loaded into part of modulated split I -> II")
  {
    useCases.undoableLoadToPart(preset, VoiceGroup::I, VoiceGroup::II);

    CHECK(sI->getModulationSource() == sII->getModulationSource());
    CHECK(sI->getModulationAmount() == sII->getModulationAmount());
  }

  WHEN("Default split loaded into part of modulated split II -> I")
  {
    useCases.undoableLoadToPart(preset, VoiceGroup::II, VoiceGroup::I);

    CHECK(sI->getModulationSource() == sII->getModulationSource());
    CHECK(sI->getModulationAmount() == sII->getModulationAmount());
  }

  WHEN("Default split loaded into part of modulated split I -> I")
  {
    useCases.undoableLoadToPart(preset, VoiceGroup::I, VoiceGroup::I);

    CHECK(sI->getModulationSource() == sII->getModulationSource());
    CHECK(sI->getModulationAmount() == sII->getModulationAmount());
  }

  WHEN("Default split loaded into part of modulated split II -> II")
  {
    useCases.undoableLoadToPart(preset, VoiceGroup::II, VoiceGroup::II);

    CHECK(sI->getModulationSource() == sII->getModulationSource());
    CHECK(sI->getModulationAmount() == sII->getModulationAmount());
  }
}

TEST_CASE("Load To Part of Split could lead to missing assignements")
{
  MockPresetStorage presets;
  TestHelper::initDualEditBuffer<SoundType::Split>();

  auto eb = TestHelper::getEditBuffer();
  auto sI = eb->findAndCastParameterByID<SplitPointParameter>({ C15::PID::Split_Split_Point, VoiceGroup::I });
  auto sII = eb->findAndCastParameterByID<SplitPointParameter>({ C15::PID::Split_Split_Point, VoiceGroup::II });

  WHEN("Split Point - I is set to C1")
  {
    ParameterUseCases useCase(sI);
    useCase.incDec(-1, false, true);

    THEN("No Gap was created moving sI explicitly")
    {
      CHECK(sI->getDisplayString() == "C1");
      CHECK(sII->getDisplayString() == "C#1");
    }

    WHEN("Default Single Sound loaded to Part II")
    {
      EditBufferUseCases useCases(eb);
      useCases.undoableLoadToPart(presets.getSinglePreset(), VoiceGroup::I, VoiceGroup::II);

      THEN("No Gap was created as Split params were not loaded")
      {
        CHECK(sI->getDisplayString() == "C1");
        CHECK(sII->getDisplayString() == "C#1");
      }
    }

    WHEN("Default Split Sound loaded to Part II")
    {
      EditBufferUseCases useCases(eb);
      useCases.undoableLoadToPart(presets.getSplitPreset(), VoiceGroup::I, VoiceGroup::II);

      THEN("No Gap was created as Split params were not loaded")
      {
        CHECK(sI->getDisplayString() == "C1");
        CHECK(sII->getDisplayString() == "C#1");
      }
    }

    WHEN("Default Layer Sound loaded to Part II")
    {
      EditBufferUseCases useCases(eb);
      useCases.undoableLoadToPart(presets.getLayerPreset(), VoiceGroup::I, VoiceGroup::II);

      THEN("No Gap was created as Split params were not loaded")
      {
        CHECK(sI->getDisplayString() == "C1");
        CHECK(sII->getDisplayString() == "C#1");
      }
    }
  }
}

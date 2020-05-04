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

using EBL = EditBufferLogicalParts;

TEST_CASE("Convert Split (II) to Single")
{
  auto voicesI = EBL::getUnisonVoice<VoiceGroup::I>();
  auto voicesII = EBL::getUnisonVoice<VoiceGroup::II>();
  auto monoII = EBL::getMonoEnable<VoiceGroup::II>();

  auto globalVolume = EBL::getMasterVolume();
  auto globalTune = EBL::getMasterTune();

  auto envAIAttack = EBL::getParameter({ 0, VoiceGroup::I });
  auto envAIIAttack = EBL::getParameter({ 0, VoiceGroup::II });

  {
    auto scope = TestHelper::createTestScope();
    auto transaction = scope->getTransaction();
    TestHelper::initDualEditBuffer<SoundType::Split>(transaction);

    voicesII->loadDefault(transaction);
    voicesII->stepCPFromHwui(transaction, 12, {});
    CHECK(voicesII->getDisplayString() == "12 voices");

    monoII->setCPFromHwui(transaction, 1);
    CHECK(monoII->getDisplayString() == "On");

    globalVolume->setCPFromHwui(transaction, 0.420);
    globalVolume->setModulationAmount(transaction, 0.3);
    globalVolume->undoableSelectModSource(transaction, MacroControls::MC1);

    globalTune->setCPFromHwui(transaction, 0.60);
    globalTune->setModulationAmount(transaction, 0.5);
    globalTune->undoableSelectModSource(transaction, MacroControls::MC2);

    EBL::getPartVolume<VoiceGroup::II>()->setCPFromHwui(transaction, 0.75);
    EBL::getPartVolume<VoiceGroup::II>()->setModulationAmount(transaction, 0.4);
    EBL::getPartVolume<VoiceGroup::II>()->undoableSelectModSource(transaction, MacroControls::MC3);

    EBL::getPartTune<VoiceGroup::II>()->setCPFromHwui(transaction, 1);
    EBL::getPartTune<VoiceGroup::II>()->setModulationAmount(transaction, 0.3);
    EBL::getPartTune<VoiceGroup::II>()->undoableSelectModSource(transaction, MacroControls::MC5);

    CHECK(EBL::getPartTune<VoiceGroup::II>()->getModulationSource() == MacroControls::MC5);
    CHECK(EBL::getPartVolume<VoiceGroup::II>()->getModulationSource() == MacroControls::MC3);

    envAIIAttack->undoableRandomize(transaction, Initiator::EXPLICIT_OTHER, 0.4);

    TestHelper::randomizeCrossFBAndToFX(transaction);
    TestHelper::randomizeFadeParams(transaction);
  }

  WHEN("Converted")
  {

    const auto mcmHash = EBL::createHashOfVector(EBL::getModMatrix());
    const auto scaleHash = EBL::createHashOfVector(EBL::getScale());

    const auto attackIICP = envAIIAttack->getControlPositionValue();

    const auto masterVolumeDisplay = globalVolume->getDisplayValue();
    const auto masterTuneDisplay = globalTune->getDisplayValue();

    const auto partTuneIIDisplay = EBL::getPartTune<VoiceGroup::II>()->getDisplayValue();
    const auto oldTuneIIModAmt = EBL::getPartTune<VoiceGroup::II>()->getModulationAmount();
    const auto oldTuneIIModSrc = EBL::getPartTune<VoiceGroup::II>()->getModulationSource();

    const auto partVolumeIIDisplay = EBL::getPartVolume<VoiceGroup::II>()->getDisplayValue();
    const auto oldVolumeIIModAmt = EBL::getPartVolume<VoiceGroup::II>()->getModulationAmount();
    const auto oldVolumeIIModSrc = EBL::getPartVolume<VoiceGroup::II>()->getModulationSource();

    const auto localNormalIIHash = EBL::createValueHash(EBL::getLocalNormal<VoiceGroup::II>());

    auto scope = TestHelper::createTestScope();
    auto transaction = scope->getTransaction();
    auto eb = TestHelper::getEditBuffer();

    eb->undoableConvertToSingle(transaction, VoiceGroup::II);

    THEN("Unison Voices Correct")
    {
      CHECK(voicesI->getDisplayString() == "12 voices");
      CHECK(EBL::getMonoEnable<VoiceGroup::I>()->getDisplayString() == "On");
    }

    THEN("Special Local Params are default")
    {
      CHECK(EBL::isDefaultLoaded(EBL::getCrossFB<VoiceGroup::I>()));
      CHECK(EBL::isDefaultLoaded(EBL::getCrossFB<VoiceGroup::II>()));
      CHECK(EBL::isDefaultLoaded(EBL::getToFX<VoiceGroup::I>()));
      CHECK(EBL::isDefaultLoaded(EBL::getToFX<VoiceGroup::II>()));
    }

    THEN("Fade is default")
    {
      CHECK(EBL::getFadeFrom<VoiceGroup::I>()->isDefaultLoaded());
      CHECK(EBL::getFadeFrom<VoiceGroup::II>()->isDefaultLoaded());
      CHECK(EBL::getFadeRange<VoiceGroup::I>()->isDefaultLoaded());
      CHECK(EBL::getFadeRange<VoiceGroup::II>()->isDefaultLoaded());
    }

    THEN("Local II copied to I")
    {
      //Check example Parameter
      CHECK_PARAMETER_CP_EQUALS_FICTION(envAIAttack, attackIICP);
      CHECK(localNormalIIHash == EBL::createValueHash(EBL::getLocalNormal<VoiceGroup::I>()));
    }

    THEN("Voices/Mono from II copied to I")
    {
      CHECK(EBL::getMonoEnable<VoiceGroup::I>()->getDisplayString() == "On");
      CHECK(EBL::getUnisonVoice<VoiceGroup::I>()->getDisplayString() == "12 voices");
    }

    THEN("Local II is default")
    {
      CHECK(EBL::isDefaultLoaded(EBL::getLocalNormal<VoiceGroup::II>()));
    }

    THEN("Voice Parameters of II are default")
    {
      CHECK(EBL::isDefaultLoaded(EBL::getMono<VoiceGroup::II>()));
      CHECK(EBL::isDefaultLoaded(EBL::getUnison<VoiceGroup::II>()));
    }

    THEN("Part Master is Default")
    {
      CHECK(EBL::isDefaultLoaded({ EBL::getPartTune<VoiceGroup::I>(), EBL::getPartTune<VoiceGroup::II>(),
                                   EBL::getPartVolume<VoiceGroup::I>(), EBL::getPartVolume<VoiceGroup::II>() }));
    }

    THEN("Split is default")
    {
      CHECK(EBL::getSplitPoint()->isDefaultLoaded());
    }

    THEN("Global Scale/MCM is unchanged")
    {
      CHECK(EBL::createHashOfVector(EBL::getScale()) == scaleHash);
      CHECK(EBL::createHashOfVector(EBL::getModMatrix()) == mcmHash);
    }

    THEN("Part and Global master have been combined")
    {
      CHECK(globalVolume->getDisplayValue() == Approx(masterVolumeDisplay + partVolumeIIDisplay));
      CHECK(globalVolume->getModulationAmount() == oldVolumeIIModAmt);
      CHECK(globalVolume->getModulationSource() == oldVolumeIIModSrc);

      CHECK(globalTune->getDisplayValue() == Approx(std::min(masterTuneDisplay + partTuneIIDisplay, 48.0)));
      CHECK(globalTune->getModulationAmount() == oldTuneIIModAmt);
      CHECK(globalTune->getModulationSource() == oldTuneIIModSrc);
    }

    THEN("EB unchanged")
    {
      CHECK_FALSE(eb->findAnyParameterChanged());
    }
  }
}

TEST_CASE("Convert Layer (II) to Single")
{
  auto voicesI = EBL::getUnisonVoice<VoiceGroup::I>();
  auto voicesII = EBL::getUnisonVoice<VoiceGroup::II>();

  auto monoI = EBL::getMonoEnable<VoiceGroup::I>();
  auto monoII = EBL::getMonoEnable<VoiceGroup::II>();

  auto globalVolume = EBL::getMasterVolume();
  auto globalTune = EBL::getMasterVolume();

  auto envAIAttack = EBL::getParameter({ 0, VoiceGroup::I });
  auto envAIIAttack = EBL::getParameter({ 0, VoiceGroup::II });

  {
    auto scope = TestHelper::createTestScope();
    auto transaction = scope->getTransaction();
    TestHelper::initDualEditBuffer<SoundType::Layer>(transaction);

    voicesI->loadDefault(transaction);
    voicesI->stepCPFromHwui(transaction, 12, {});
    CHECK(voicesI->getDisplayString() == "12 voices");

    monoI->setCPFromHwui(transaction, 1);
    CHECK(monoI->getDisplayString() == "On");

    globalTune->setCPFromHwui(transaction, 0.25);
    globalVolume->setModulationAmount(transaction, 0.5);
    globalVolume->undoableSelectModSource(transaction, MacroControls::MC1);

    globalTune->setCPFromHwui(transaction, 0.5);
    globalTune->setModulationAmount(transaction, 0.5);
    globalTune->undoableSelectModSource(transaction, MacroControls::MC2);

    EBL::getPartVolume<VoiceGroup::II>()->setModulationAmount(transaction, 0.4);
    EBL::getPartVolume<VoiceGroup::II>()->undoableSelectModSource(transaction, MacroControls::MC3);
    EBL::getPartTune<VoiceGroup::II>()->setModulationAmount(transaction, 0.4);
    EBL::getPartTune<VoiceGroup::II>()->undoableSelectModSource(transaction, MacroControls::MC5);

    envAIAttack->undoableRandomize(transaction, Initiator::EXPLICIT_OTHER, 0.4);
    envAIIAttack->undoableRandomize(transaction, Initiator::EXPLICIT_OTHER, 0.4);

    TestHelper::randomizeCrossFBAndToFX(transaction);
    TestHelper::randomizeFadeParams(transaction);
  }

  WHEN("Converted")
  {
    const auto mcmHash = EBL::createHashOfVector(EBL::getModMatrix());
    const auto scaleHash = EBL::createHashOfVector(EBL::getScale());

    const auto oldAttackIICP = envAIIAttack->getControlPositionValue();

    const auto oldMasterVolumeDisplay = EBL::getMasterVolume()->getDisplayValue();
    const auto oldMasterTuneDisplay = EBL::getMasterVolume()->getDisplayValue();

    const auto oldPartTuneIIDisplay = EBL::getPartTune<VoiceGroup::II>()->getDisplayValue();
    const auto oldPartVolumeIIDisplay = EBL::getPartVolume<VoiceGroup::II>()->getDisplayValue();

    const auto oldPartTuneModSrc = EBL::getPartTune<VoiceGroup::II>()->getModulationSource();
    const auto oldPartTuneModAmt = EBL::getPartTune<VoiceGroup::II>()->getModulationAmount();

    const auto oldPartVolumeModSrc = EBL::getPartVolume<VoiceGroup::II>()->getModulationSource();
    const auto oldPartVolumeModAmt = EBL::getPartVolume<VoiceGroup::II>()->getModulationAmount();

    auto scope = TestHelper::createTestScope();
    auto transaction = scope->getTransaction();
    auto eb = TestHelper::getEditBuffer();

    eb->undoableConvertToSingle(transaction, VoiceGroup::II);

    THEN("Unison Voices Correct")
    {
      CHECK(voicesI->getDisplayString() == "12 voices");
      CHECK(EBL::getMonoEnable<VoiceGroup::I>()->getDisplayString() == "On");

      CHECK(EBL::getUnisonVoice<VoiceGroup::II>()->isDefaultLoaded());
      CHECK(EBL::getMonoEnable<VoiceGroup::II>()->isDefaultLoaded());
    }

    THEN("Special Local Params are default")
    {
      CHECK(EBL::isDefaultLoaded(EBL::getCrossFB<VoiceGroup::I>(), EBL::getCrossFB<VoiceGroup::II>()));
      CHECK(EBL::isDefaultLoaded(EBL::getToFX<VoiceGroup::I>(), EBL::getToFX<VoiceGroup::II>()));
    }

    THEN("Local I has Local II and Local II is default")
    {
      CHECK_PARAMETER_CP_EQUALS_FICTION(envAIAttack, oldAttackIICP);
      CHECK(EBL::isDefaultLoaded(EBL::getLocalNormal<VoiceGroup::II>()));
    }

    THEN("Fade is default")
    {
      CHECK(EBL::getFadeFrom<VoiceGroup::I>()->isDefaultLoaded());
      CHECK(EBL::getFadeFrom<VoiceGroup::II>()->isDefaultLoaded());
      CHECK(EBL::getFadeRange<VoiceGroup::I>()->isDefaultLoaded());
      CHECK(EBL::getFadeRange<VoiceGroup::II>()->isDefaultLoaded());
    }

    THEN("Part Master was copied from I")
    {
      CHECK(EBL::getPartTune<VoiceGroup::I>()->getDisplayValue() == oldPartTuneIIDisplay + oldMasterTuneDisplay);
      CHECK(EBL::getPartVolume<VoiceGroup::I>()->getDisplayValue() == oldPartVolumeIIDisplay + oldMasterVolumeDisplay);

      CHECK(EBL::getMasterVolume()->getModulationAmount() == oldPartVolumeModAmt);
      CHECK(EBL::getMasterTune()->getModulationAmount() == oldPartTuneModAmt);

      CHECK(EBL::getMasterVolume()->getModulationSource() == oldPartVolumeModSrc);
      CHECK(EBL::getMasterTune()->getModulationSource() == oldPartTuneModSrc);

      CHECK(EBL::isDefaultLoaded(EBL::getPartMaster<VoiceGroup::I>(), EBL::getPartMaster<VoiceGroup::II>()));
    }

    THEN("Split is default")
    {
      CHECK(EBL::getSplitPoint()->isDefaultLoaded());
    }

    THEN("Global Scale/MCM is unchanged")
    {
      CHECK(EBL::createHashOfVector(EBL::getScale()) == scaleHash);
      CHECK(EBL::createHashOfVector(EBL::getModMatrix()) == mcmHash);
    }

    THEN("EB unchanged")
    {
      CHECK_FALSE(eb->findAnyParameterChanged());
    }
  }
}

TEST_CASE("Convert Layer (I) to Single")
{
  auto voicesI = EBL::getUnisonVoice<VoiceGroup::I>();
  auto voicesII = EBL::getUnisonVoice<VoiceGroup::II>();

  auto monoI = EBL::getMonoEnable<VoiceGroup::I>();
  auto monoII = EBL::getMonoEnable<VoiceGroup::II>();

  auto globalVolume = EBL::getMasterVolume();
  auto globalTune = EBL::getMasterVolume();

  auto envAIAttack = EBL::getParameter({ 0, VoiceGroup::I });
  auto envAIIAttack = EBL::getParameter({ 0, VoiceGroup::II });

  {
    auto scope = TestHelper::createTestScope();
    auto transaction = scope->getTransaction();
    TestHelper::initDualEditBuffer<SoundType::Layer>(transaction);

    voicesI->loadDefault(transaction);
    voicesI->stepCPFromHwui(transaction, 12, {});
    CHECK(voicesI->getDisplayString() == "12 voices");

    monoI->setCPFromHwui(transaction, 1);
    CHECK(monoI->getDisplayString() == "On");

    globalTune->setCPFromHwui(transaction, 0.25);
    globalVolume->setModulationAmount(transaction, 0.5);
    globalVolume->undoableSelectModSource(transaction, MacroControls::MC1);

    globalTune->setCPFromHwui(transaction, 0.5);
    globalTune->setModulationAmount(transaction, 0.5);
    globalTune->undoableSelectModSource(transaction, MacroControls::MC2);

    EBL::getPartVolume<VoiceGroup::I>()->setModulationAmount(transaction, 0.4);
    EBL::getPartVolume<VoiceGroup::I>()->undoableSelectModSource(transaction, MacroControls::MC3);
    EBL::getPartTune<VoiceGroup::I>()->setModulationAmount(transaction, 0.4);
    EBL::getPartTune<VoiceGroup::I>()->undoableSelectModSource(transaction, MacroControls::MC5);

    envAIAttack->undoableRandomize(transaction, Initiator::EXPLICIT_OTHER, 0.4);
    envAIIAttack->undoableRandomize(transaction, Initiator::EXPLICIT_OTHER, 0.4);

    TestHelper::randomizeCrossFBAndToFX(transaction);
    TestHelper::randomizeFadeParams(transaction);
  }

  WHEN("Converted")
  {
    const auto mcmHash = EBL::createHashOfVector(EBL::getModMatrix());
    const auto scaleHash = EBL::createHashOfVector(EBL::getScale());

    const auto oldAttackICP = envAIAttack->getControlPositionValue();

    const auto oldMasterVolumeDisplay = EBL::getMasterVolume()->getDisplayValue();
    const auto oldMasterTuneDisplay = EBL::getMasterVolume()->getDisplayValue();

    const auto oldPartTuneIDisplay = EBL::getPartTune<VoiceGroup::I>()->getDisplayValue();
    const auto oldPartVolumeIDisplay = EBL::getPartVolume<VoiceGroup::I>()->getDisplayValue();

    const auto oldPartTuneModSrc = EBL::getPartTune<VoiceGroup::I>()->getModulationSource();
    const auto oldPartTuneModAmt = EBL::getPartTune<VoiceGroup::I>()->getModulationAmount();

    const auto oldPartVolumeModSrc = EBL::getPartVolume<VoiceGroup::I>()->getModulationSource();
    const auto oldPartVolumeModAmt = EBL::getPartVolume<VoiceGroup::I>()->getModulationAmount();

    auto scope = TestHelper::createTestScope();
    auto transaction = scope->getTransaction();
    auto eb = TestHelper::getEditBuffer();

    eb->undoableConvertToSingle(transaction, VoiceGroup::I);

    THEN("Unison Voices Correct")
    {
      CHECK(voicesI->getDisplayString() == "12 voices");
      CHECK(EBL::getMonoEnable<VoiceGroup::I>()->getDisplayString() == "On");

      CHECK(EBL::getUnisonVoice<VoiceGroup::II>()->isDefaultLoaded());
      CHECK(EBL::getMonoEnable<VoiceGroup::II>()->isDefaultLoaded());
    }

    THEN("Special Local Params are default")
    {
      CHECK(EBL::isDefaultLoaded(EBL::getCrossFB<VoiceGroup::I>(), EBL::getCrossFB<VoiceGroup::II>()));
      CHECK(EBL::isDefaultLoaded(EBL::getToFX<VoiceGroup::I>(), EBL::getToFX<VoiceGroup::II>()));
    }

    THEN("Local I has Local II and Local II is default")
    {
      CHECK_PARAMETER_CP_EQUALS_FICTION(envAIAttack, oldAttackICP);
      CHECK(EBL::isDefaultLoaded(EBL::getLocalNormal<VoiceGroup::II>()));
    }

    THEN("Fade is default")
    {
      CHECK(EBL::getFadeFrom<VoiceGroup::I>()->isDefaultLoaded());
      CHECK(EBL::getFadeFrom<VoiceGroup::II>()->isDefaultLoaded());
      CHECK(EBL::getFadeRange<VoiceGroup::I>()->isDefaultLoaded());
      CHECK(EBL::getFadeRange<VoiceGroup::II>()->isDefaultLoaded());
    }

    THEN("Part Master was copied from I")
    {
      CHECK(EBL::getPartTune<VoiceGroup::I>()->getDisplayValue() == oldPartTuneIDisplay + oldMasterTuneDisplay);
      CHECK(EBL::getPartVolume<VoiceGroup::I>()->getDisplayValue() == oldPartVolumeIDisplay + oldMasterVolumeDisplay);

      CHECK(EBL::getMasterVolume()->getModulationAmount() == oldPartVolumeModAmt);
      CHECK(EBL::getMasterTune()->getModulationAmount() == oldPartTuneModAmt);

      CHECK(EBL::getMasterVolume()->getModulationSource() == oldPartVolumeModSrc);
      CHECK(EBL::getMasterTune()->getModulationSource() == oldPartTuneModSrc);

      CHECK(EBL::isDefaultLoaded(EBL::getPartMaster<VoiceGroup::I>(), EBL::getPartMaster<VoiceGroup::II>()));
    }

    THEN("Split is default")
    {
      CHECK(EBL::getSplitPoint()->isDefaultLoaded());
    }

    THEN("Global Scale/MCM is unchanged")
    {
      CHECK(EBL::createHashOfVector(EBL::getScale()) == scaleHash);
      CHECK(EBL::createHashOfVector(EBL::getModMatrix()) == mcmHash);
    }

    THEN("EB unchanged")
    {
      CHECK_FALSE(eb->findAnyParameterChanged());
    }
  }
}
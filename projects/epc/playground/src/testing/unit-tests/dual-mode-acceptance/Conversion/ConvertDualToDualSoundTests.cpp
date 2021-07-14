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

TEST_CASE("Convert Split (II) to Layer")
{
  auto voicesI = EBL::getUnisonVoice<VoiceGroup::I>();
  auto voicesII = EBL::getUnisonVoice<VoiceGroup::II>();
  auto monoII = EBL::getMonoEnable<VoiceGroup::II>();

  auto attI = EBL::getParameter({ 0, VoiceGroup::I });
  auto attII = EBL::getParameter({ 0, VoiceGroup::II });

  {
    auto scope = TestHelper::createTestScope();
    TestHelper::initDualEditBuffer<SoundType::Split>(scope->getTransaction());
    voicesII->loadDefault(scope->getTransaction(), Defaults::FactoryDefault);
    voicesII->stepCPFromHwui(scope->getTransaction(), 12, {});
    CHECK(voicesII->getDisplayString() == "12 voices");

    monoII->setCPFromHwui(scope->getTransaction(), 1);
    CHECK(monoII->getDisplayString() == "On");

    Application::get().getHWUI()->setCurrentVoiceGroup(VoiceGroup::II);

    for(auto p : EBL::getLocalNormal<VoiceGroup::I>())
      TestHelper::forceParameterChange(scope->getTransaction(), p);

    for(auto p : EBL::getLocalNormal<VoiceGroup::II>())
      TestHelper::forceParameterChange(scope->getTransaction(), p);

    for(auto p : EBL::getPartMaster<VoiceGroup::I>())
      TestHelper::forceParameterChange(scope->getTransaction(), p);

    for(auto p : EBL::getPartMaster<VoiceGroup::II>())
      TestHelper::forceParameterChange(scope->getTransaction(), p);

    for(auto p : EBL::getToFX<VoiceGroup::I>())
      TestHelper::forceParameterChange(scope->getTransaction(), p);

    for(auto p : EBL::getToFX<VoiceGroup::II>())
      TestHelper::forceParameterChange(scope->getTransaction(), p);

    attI->setCPFromHwui(scope->getTransaction(), 0.187);
    attII->setCPFromHwui(scope->getTransaction(), 0.287);
  }

  WHEN("Converted")
  {
    const auto localIHash = EBL::createValueHash(EBL::getLocalNormal<VoiceGroup::I>());
    const auto localIIHash = EBL::createValueHash(EBL::getLocalNormal<VoiceGroup::II>());

    const auto splitCP = EBL::getSplitPoint<VoiceGroup::I>()->getControlPositionValue();

    const auto mcmHash = EBL::createHashOfVector(EBL::getModMatrix());
    const auto scaleHash = EBL::createHashOfVector(EBL::getScale());

    const auto masterVolumeHash = EBL::getMasterVolume()->getHash();
    const auto masterTuneHash = EBL::getMasterTune()->getHash();

    const auto oldToFxI = EBL::createValueHash(EBL::getToFX<VoiceGroup::I>());
    const auto oldToFxII = EBL::createValueHash(EBL::getToFX<VoiceGroup::II>());

    const auto oldPartMasterHashI = EBL::createValueHash(EBL::getPartMaster<VoiceGroup::I>());
    const auto oldPartMasterHashII = EBL::createValueHash(EBL::getPartMaster<VoiceGroup::II>());

    auto scope = TestHelper::createTestScope();
    auto transaction = scope->getTransaction();
    auto eb = TestHelper::getEditBuffer();
    eb->undoableConvertToDual(transaction, SoundType::Layer);

    THEN("Unison Voices Correct")
    {
      CHECK(voicesI->getDisplayString() == "12 voices");
      CHECK(voicesII->getDisplayString() == "1 voice (off)");
    }

    THEN("Cross FB I/II default")
    {
      CHECK(EBL::isFactoryDefaultLoaded(EBL::getCrossFB<VoiceGroup::I>()));
      CHECK(EBL::isFactoryDefaultLoaded(EBL::getCrossFB<VoiceGroup::II>()));
    }

    THEN("to FX unchanged")
    {
      CHECK(EBL::createValueHash(EBL::getToFX<VoiceGroup::I>()) == oldToFxI);
      CHECK(EBL::createValueHash(EBL::getToFX<VoiceGroup::II>()) == oldToFxII);
    }

    THEN("Local normal unchanged")
    {
      CHECK(EBL::createValueHash(EBL::getLocalNormal<VoiceGroup::I>()) == localIHash);
      CHECK(EBL::createValueHash(EBL::getLocalNormal<VoiceGroup::II>()) == localIIHash);
      CHECK_PARAMETER_CP_EQUALS_FICTION(attI, 0.187);
      CHECK_PARAMETER_CP_EQUALS_FICTION(attII, 0.287);
    }

    THEN("Voices/Mono from II copied to I")
    {
      CHECK(EBL::getMonoEnable<VoiceGroup::I>()->getDisplayString() == "On");
      CHECK(EBL::getUnisonVoice<VoiceGroup::I>()->getDisplayString() == "12 voices");
    }

    THEN("Part Master is Unchanged")
    {
      CHECK(EBL::createValueHash(EBL::getPartMaster<VoiceGroup::I>()) == oldPartMasterHashI);
      CHECK(EBL::createValueHash(EBL::getPartMaster<VoiceGroup::II>()) == oldPartMasterHashII);
    }

    THEN("Voice Parameters of II are default")
    {
      for(auto& p : EBL::getMono<VoiceGroup::II>())
      {
        CHECK(p->isFactoryDefaultLoaded());
      }

      for(auto& p : EBL::getUnison<VoiceGroup::II>())
      {
        CHECK(p->isFactoryDefaultLoaded());
      }
    }

    THEN("Fade is Default")
    {
      CHECK(EBL::getFadeFrom<VoiceGroup::I>()->isFactoryDefaultLoaded());
      CHECK(EBL::getFadeFrom<VoiceGroup::II>()->isFactoryDefaultLoaded());
      CHECK(EBL::getFadeRange<VoiceGroup::I>()->isFactoryDefaultLoaded());
      CHECK(EBL::getFadeRange<VoiceGroup::II>()->isFactoryDefaultLoaded());
    }

    THEN("Split is default")
    {
      CHECK(EBL::getSplitPoint<VoiceGroup::I>()->isFactoryDefaultLoaded());
      CHECK(EBL::getSplitPoint<VoiceGroup::II>()->isFactoryDefaultLoaded());
    }

    THEN("Global Master/Scale/MCM is unchanged")
    {
      CHECK(EBL::getMasterVolume()->getHash() == masterVolumeHash);
      CHECK(EBL::getMasterTune()->getHash() == masterTuneHash);
      CHECK(EBL::createHashOfVector(EBL::getScale()) == scaleHash);
      CHECK(EBL::createHashOfVector(EBL::getModMatrix()) == mcmHash);
    }

    THEN("EB unchanged")
    {
      CHECK_FALSE(eb->findAnyParameterChanged());
    }
  }
}

TEST_CASE("Convert Layer I to Split")
{
  auto voicesI = EBL::getUnisonVoice<VoiceGroup::I>();
  auto monoI = EBL::getMonoEnable<VoiceGroup::I>();
  auto globalVolume = EBL::getMasterVolume();
  auto globalTune = EBL::getMasterVolume();

  auto envAIAttack = EBL::getParameter({ 0, VoiceGroup::I });
  auto envAIIAttack = EBL::getParameter({ 0, VoiceGroup::II });

  {
    auto scope = TestHelper::createTestScope();
    auto transaction = scope->getTransaction();
    TestHelper::initDualEditBuffer<SoundType::Layer>(transaction);

    voicesI->loadDefault(transaction, Defaults::FactoryDefault);
    voicesI->stepCPFromHwui(transaction, 12, {});
    CHECK(voicesI->getDisplayString() == "12 voices");

    monoI->setCPFromHwui(transaction, 1);
    CHECK(monoI->getDisplayString() == "On");

    globalVolume->setModulationAmount(transaction, 0.5);
    globalVolume->undoableSelectModSource(transaction, MacroControls::MC1);

    globalTune->setModulationAmount(transaction, 0.5);
    globalTune->undoableSelectModSource(transaction, MacroControls::MC2);

    EBL::getFadeFrom<VoiceGroup::I>()->setCPFromHwui(transaction, 0.187);
    EBL::getFadeFrom<VoiceGroup::II>()->setCPFromHwui(transaction, 0.887);

    EBL::getPartVolume<VoiceGroup::I>()->setCPFromHwui(transaction, 0.25);
    EBL::getPartVolume<VoiceGroup::I>()->setModulationAmount(transaction, 0.4);
    EBL::getPartVolume<VoiceGroup::I>()->undoableSelectModSource(transaction, MacroControls::MC3);

    EBL::getPartTune<VoiceGroup::I>()->setCPFromHwui(transaction, 0.25);
    EBL::getPartTune<VoiceGroup::I>()->setModulationAmount(transaction, 0.4);
    EBL::getPartTune<VoiceGroup::I>()->undoableSelectModSource(transaction, MacroControls::MC5);

    envAIAttack->undoableRandomize(transaction, Initiator::EXPLICIT_OTHER, 0.4);
    envAIIAttack->undoableRandomize(transaction, Initiator::EXPLICIT_OTHER, 0.4);

    EBL::getFadeFrom<VoiceGroup::I>()->setCPFromHwui(transaction, 0.7);
    EBL::getFadeFrom<VoiceGroup::II>()->setCPFromHwui(transaction, 0.7);

    TestHelper::randomizeCrossFBAndToFX(transaction);
  }

  WHEN("Converted")
  {
    const auto mcmHash = EBL::createHashOfVector(EBL::getModMatrix());
    const auto scaleHash = EBL::createHashOfVector(EBL::getScale());

    const auto attackICP = envAIAttack->getControlPositionValue();
    const auto attackIICP = envAIIAttack->getControlPositionValue();

    const auto masterVolumeDisplay = EBL::getMasterVolume()->getDisplayValue();
    const auto masterTuneDisplay = EBL::getMasterVolume()->getDisplayValue();

    const auto oldFromI = EBL::getFadeFrom<VoiceGroup::I>()->getControlPositionValue();
    const auto oldFromII = EBL::getFadeFrom<VoiceGroup::II>()->getControlPositionValue();

    const auto oldLocalIHash = EBL::createHashOfVector(EBL::getLocalNormal<VoiceGroup::I>());
    const auto oldLocalIIHash = EBL::createHashOfVector(EBL::getLocalNormal<VoiceGroup::II>());

    const auto partIMasterHash = EBL::createValueHash(EBL::getPartMaster<VoiceGroup::I>());
    const auto partIIMasterHash = EBL::createValueHash(EBL::getPartMaster<VoiceGroup::II>());

    const auto oldToFXIHash = EBL::createValueHash(EBL::getToFX<VoiceGroup::I>());
    const auto oldToFXIIHash = EBL::createValueHash(EBL::getToFX<VoiceGroup::II>());

    auto scope = TestHelper::createTestScope();
    auto transaction = scope->getTransaction();
    auto eb = TestHelper::getEditBuffer();
    eb->undoableConvertToDual(transaction, SoundType::Split);

    THEN("Unison Voices Correct")
    {
      CHECK(voicesI->getDisplayString() == "12 voices");
      CHECK(EBL::getMonoEnable<VoiceGroup::I>()->getDisplayString() == "On");

      CHECK(EBL::getUnisonVoice<VoiceGroup::II>()->getDisplayString() == "12 voices");
      CHECK(EBL::getMonoEnable<VoiceGroup::II>()->getDisplayString() == "On");
    }

    THEN("CrossFB Params are default")
    {
      CHECK(EBL::isFactoryDefaultLoaded(EBL::getCrossFB<VoiceGroup::I>()));
      CHECK(EBL::isFactoryDefaultLoaded(EBL::getCrossFB<VoiceGroup::II>()));
    }

    THEN("To FX Unchanged")
    {
      CHECK(EBL::createValueHash(EBL::getToFX<VoiceGroup::I>()) == oldToFXIHash);
      CHECK(EBL::createValueHash(EBL::getToFX<VoiceGroup::II>()) == oldToFXIIHash);
    }

    THEN("Local kept values")
    {
      CHECK_PARAMETER_CP_EQUALS_FICTION(envAIAttack, attackICP);
      CHECK_PARAMETER_CP_EQUALS_FICTION(envAIIAttack, attackIICP);

      CHECK(EBL::createHashOfVector(EBL::getLocalNormal<VoiceGroup::I>()) == oldLocalIHash);
      CHECK(EBL::createHashOfVector(EBL::getLocalNormal<VoiceGroup::II>()) == oldLocalIIHash);
    }

    THEN("Fade is default")
    {
      CHECK(EBL::getFadeFrom<VoiceGroup::I>()->isFactoryDefaultLoaded());
      CHECK(EBL::getFadeFrom<VoiceGroup::II>()->isFactoryDefaultLoaded());
      CHECK(EBL::getFadeRange<VoiceGroup::I>()->isFactoryDefaultLoaded());
      CHECK(EBL::getFadeRange<VoiceGroup::II>()->isFactoryDefaultLoaded());
    }

    THEN("Part Master was copied from I")
    {
      CHECK(EBL::createValueHash(EBL::getPartMaster<VoiceGroup::I>()) == partIMasterHash);
      CHECK(EBL::createValueHash(EBL::getPartMaster<VoiceGroup::II>()) == partIIMasterHash);
    }

    THEN("Split is mean of fade from I and II")
    {
      CHECK_PARAMETER_CP_EQUALS_FICTION(EBL::getSplitPoint<VoiceGroup::I>(), (oldFromI + oldFromII) / 2.0);
      auto II = EBL::getSplitPoint<VoiceGroup::I>()->getValue().getNextStepValue((oldFromI + oldFromII) / 2.0, 1, false,
                                                                                 false);
      CHECK_PARAMETER_CP_EQUALS_FICTION(EBL::getSplitPoint<VoiceGroup::II>(), II);
    }

    THEN("Global Scale/MCM is unchanged")
    {
      CHECK(EBL::createHashOfVector(EBL::getScale()) == scaleHash);
      CHECK(EBL::createHashOfVector(EBL::getModMatrix()) == mcmHash);
    }

    THEN("Global master did not changed")
    {
      CHECK(EBL::getMasterVolume()->getDisplayValue() == masterVolumeDisplay);
      CHECK(EBL::getMasterTune()->getDisplayValue() == masterTuneDisplay);
    }

    THEN("EB unchanged")
    {
      CHECK_FALSE(eb->findAnyParameterChanged());
    }
  }
}

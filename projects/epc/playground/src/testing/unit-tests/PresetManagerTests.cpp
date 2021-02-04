#include <catch.hpp>
#include "testing/TestHelper.h"
#include "mock/MockPresetStorage.h"
#include <presets/Bank.h>
#include <presets/Preset.h>
#include <presets/EditBuffer.h>
#include <device-settings/Settings.h>
#include <device-settings/ExternalMidiEnabledSetting.h>

namespace Helper
{

  void clearPresetManager()
  {
    auto pm = TestHelper::getPresetManager();
    auto scope = TestHelper::createTestScope();
    pm->clear(scope->getTransaction());
    REQUIRE(pm->getNumBanks() == 0);
  }

  Bank* getBank(const Preset* p)
  {
    auto bank = dynamic_cast<Bank*>(p->getParent());
    REQUIRE(bank != nullptr);
    return bank;
  }
}

TEST_CASE("Preset Manager Init")
{
  auto pm = TestHelper::getPresetManager();
  REQUIRE(pm != nullptr);
}

TEST_CASE("Create Bank")
{
  auto pm = TestHelper::getPresetManager();
  Helper::clearPresetManager();

  SECTION("New Bank")
  {
    auto scope = TestHelper::createTestScope();
    auto newBank = pm->addBank(scope->getTransaction());
    REQUIRE(newBank != nullptr);
    REQUIRE(pm->findBank(newBank->getUuid()) == newBank);
    REQUIRE(pm->getNumBanks() == 1);
  }
}

namespace EditBufferHelper
{
  template <SoundType tSoundType> void convertEditBufferToDual()
  {
    auto scope = TestHelper::createTestScope();
    auto eb = TestHelper::getEditBuffer();

    eb->undoableConvertToDual(scope->getTransaction(), tSoundType);
    REQUIRE(eb->getType() == tSoundType);
  }

  void convertEditBufferToSingle()
  {
    auto scope = TestHelper::createTestScope();
    auto eb = TestHelper::getEditBuffer();

    eb->undoableConvertToSingle(scope->getTransaction(), VoiceGroup::I);
    REQUIRE(eb->getType() == SoundType::Single);
  }

  void overwritePresetWithEditBuffer(Preset* p)
  {
    auto scope = TestHelper::createTestScope();
    auto eb = TestHelper::getEditBuffer();

    auto ebType = eb->getType();
    p->copyFrom(scope->getTransaction(), eb);
    REQUIRE(p->getType() == ebType);
  }
}
TEST_CASE("Overwrite Presets")
{
  Helper::clearPresetManager();
  MockPresetStorage presets;

  SECTION("Overwrite Single with Single")
  {
    EditBufferHelper::convertEditBufferToSingle();
    EditBufferHelper::overwritePresetWithEditBuffer(presets.getSinglePreset());
  }

  SECTION("Overwrite Single with Split")
  {
    EditBufferHelper::convertEditBufferToDual<SoundType::Split>();
    EditBufferHelper::overwritePresetWithEditBuffer(presets.getSinglePreset());
  }

  SECTION("Overwrite Single with Layer")
  {
    EditBufferHelper::convertEditBufferToDual<SoundType::Layer>();
    EditBufferHelper::overwritePresetWithEditBuffer(presets.getSinglePreset());
  }

  SECTION("Overwrite Layer with Single")
  {
    EditBufferHelper::convertEditBufferToSingle();
    EditBufferHelper::overwritePresetWithEditBuffer(presets.getLayerPreset());
  }

  SECTION("Overwrite Layer with Split")
  {
    EditBufferHelper::convertEditBufferToDual<SoundType::Split>();
    EditBufferHelper::overwritePresetWithEditBuffer(presets.getLayerPreset());
  }

  SECTION("Overwrite Layer with Layer")
  {
    EditBufferHelper::convertEditBufferToDual<SoundType::Layer>();
    EditBufferHelper::overwritePresetWithEditBuffer(presets.getLayerPreset());
  }

  SECTION("Overwrite Split with Single")
  {
    EditBufferHelper::convertEditBufferToSingle();
    EditBufferHelper::overwritePresetWithEditBuffer(presets.getSplitPreset());
  }

  SECTION("Overwrite Split with Split")
  {
    EditBufferHelper::convertEditBufferToDual<SoundType::Split>();
    EditBufferHelper::overwritePresetWithEditBuffer(presets.getSplitPreset());
  }

  SECTION("Overwrite Split with Layer")
  {
    EditBufferHelper::convertEditBufferToDual<SoundType::Layer>();
    EditBufferHelper::overwritePresetWithEditBuffer(presets.getSplitPreset());
  }
}

TEST_CASE("Midi Selection Sends Signals Appropriately")
{
  Helper::clearPresetManager();
  MockPresetStorage presets;
  MockPresetStorage presets2;
  auto pm = TestHelper::getPresetManager();

  auto midiEnable = Application::get().getSettings()->getSetting<ExternalMidiEnabledSetting>()->scopedOverlay(
      BooleanSettings::BOOLEAN_SETTING_TRUE);

  CHECK(pm->getNumBanks() == 2);

  WHEN("Midi Bank Selected")
  {
    const auto oldMidiUuid = pm->getMidiSelectedBank();
    Uuid receivedMidiUuid;

    auto c = pm->onMidiBankSelectionHappened([&](Uuid id) { receivedMidiUuid = id; });

    {
      auto scope = TestHelper::createTestScope();
      auto transaction = scope->getTransaction();
      pm->selectMidiBank(transaction, presets.getBank()->getUuid());
    }

    CHECK(presets.getBank()->getUuid() == receivedMidiUuid);

    THEN("UNDO")
    {
      pm->getUndoScope().undo();
      CHECK(oldMidiUuid == receivedMidiUuid);

      THEN("REDO")
      {
        pm->getUndoScope().redo();
        CHECK(presets.getBank()->getUuid() == receivedMidiUuid);
        CHECK(pm->findMidiSelectedBank() != nullptr);
      }
    }
    c.disconnect();
  }
}

TEST_CASE("Delete Current Midi Bank resets attribute")
{
  Helper::clearPresetManager();
  auto pm = TestHelper::getPresetManager();
  PresetManagerUseCases useCases(pm);

  auto midiEnable = Application::get().getSettings()->getSetting<ExternalMidiEnabledSetting>()->scopedOverlay(
      BooleanSettings::BOOLEAN_SETTING_TRUE);

  //Create bank to use
  useCases.createBankAndStoreEditBuffer();
  auto b = pm->getSelectedBank();
  CHECK(pm->getNumBanks() == 1);
  CHECK(b != nullptr);

  //select bank as midi target
  useCases.selectMidiBank(b);
  CHECK(pm->getMidiSelectedBank() == b->getUuid());

  WHEN("Bank Deleted")
  {
    useCases.deleteBank(b);
    CHECK(pm->getNumBanks() == 0);
    CHECK(pm->getMidiSelectedBank() == Uuid::none());
  }
}

TEST_CASE("When Midi Bank is Selected new and old midi selection banks get notified via onChange")
{
  Helper::clearPresetManager();
  auto pm = TestHelper::getPresetManager();
  PresetManagerUseCases useCases(pm);

  useCases.createBankAndStoreEditBuffer();
  auto b1 = pm->getSelectedBank();

  useCases.createBankAndStoreEditBuffer();
  auto b2 = pm->getSelectedBank();

  int changesB1 = 0;
  int changesB2 = 0;
  auto c1 = b1->onBankChanged([&]() { changesB1++; });
  auto c2 = b2->onBankChanged([&]() { changesB2++; });

  WHEN("Midi Bank 1 Selected")
  {
    const auto b1ChangesBeforeSelection = changesB1;
    useCases.selectMidiBank(b1);
    CHECK(changesB1 > b1ChangesBeforeSelection);
    const auto b1ChangesAfterSelection = changesB1;

    THEN("Midi Bank 2 Selected")
    {
      const auto b2ChangesBeforeSelection = changesB2;
      useCases.selectMidiBank(b2);
      CHECK(changesB2 > b2ChangesBeforeSelection);
      CHECK(changesB1 > b1ChangesAfterSelection);
    }
  }

  c1.disconnect();
  c2.disconnect();
}
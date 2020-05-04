#include <catch.hpp>
#include "testing/TestHelper.h"
#include "mock/MockPresetStorage.h"
#include <presets/Bank.h>
#include <presets/Preset.h>
#include <presets/EditBuffer.h>

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
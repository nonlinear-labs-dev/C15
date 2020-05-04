#include <testing/TestHelper.h>
#include <testing/unit-tests/mock/EditBufferNamedLogicalParts.h>
#include <testing/unit-tests/mock/MockPresetStorage.h>
#include <proxies/hwui/HWUI.h>

TEST_CASE("FROM Layer")
{
  MockPresetStorage presets;

  auto eb = TestHelper::getEditBuffer();
  eb->undoableLoad(presets.getLayerPreset());

  Application::get().getHWUI()->setCurrentVoiceGroup(VoiceGroup::II);

  CHECK(eb->getType() == SoundType::Layer);

  WHEN("FB A/B x selected")
  {
    eb->undoableSelectParameter({ 346, VoiceGroup::II });

    THEN("Single Preset Loaded")
    {
      eb->undoableLoad(presets.getSinglePreset());
      CHECK(eb->getSelected()->getID() == ParameterId { 346, VoiceGroup::I });
    }

    THEN("Split Preset Loaded")
    {
      eb->undoableLoad(presets.getSplitPreset());
      CHECK(eb->getSelected()->getID() == ParameterId { 346, VoiceGroup::II });
    }

    THEN("Split converted")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToDual(scope->getTransaction(), SoundType::Split);
      CHECK(eb->getSelected()->getID() == ParameterId { 346, VoiceGroup::II });
    }

    THEN("Single converted")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToSingle(scope->getTransaction(), VoiceGroup::II);
      CHECK(eb->getSelected()->getID() == ParameterId { 346, VoiceGroup::I });
    }
  }

  WHEN("FB A/B x - A-B selected")
  {
    eb->undoableSelectParameter({ 348, VoiceGroup::II });

    THEN("Single Preset Loaded")
    {
      eb->undoableLoad(presets.getSinglePreset());
      CHECK(eb->getSelected()->getID() == ParameterId { 346, VoiceGroup::I });
    }

    THEN("Split Preset Loaded")
    {
      eb->undoableLoad(presets.getSplitPreset());
      CHECK(eb->getSelected()->getID() == ParameterId { 346, VoiceGroup::II });
    }

    THEN("Split converted")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToDual(scope->getTransaction(), SoundType::Split);
      CHECK(eb->getSelected()->getID() == ParameterId { 346, VoiceGroup::II });
    }

    THEN("Single converted")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToSingle(scope->getTransaction(), VoiceGroup::II);
      CHECK(eb->getSelected()->getID() == ParameterId { 346, VoiceGroup::I });
    }
  }

  WHEN("FB: Comb - From x selected")
  {
    eb->undoableSelectParameter({ 350, VoiceGroup::II });

    THEN("Single Preset Loaded")
    {
      eb->undoableLoad(presets.getSinglePreset());
      CHECK(eb->getSelected()->getID() == ParameterId { 156, VoiceGroup::I });
    }

    THEN("Split Preset Loaded")
    {
      eb->undoableLoad(presets.getSplitPreset());
      CHECK(eb->getSelected()->getID() == ParameterId { 156, VoiceGroup::II });
    }

    THEN("Split converted")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToDual(scope->getTransaction(), SoundType::Split);
      CHECK(eb->getSelected()->getID() == ParameterId { 156, VoiceGroup::II });
    }

    THEN("Single converted")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToSingle(scope->getTransaction(), VoiceGroup::II);
      CHECK(eb->getSelected()->getID() == ParameterId { 156, VoiceGroup::I });
    }
  }

  WHEN("FB: SV Filter - From x selected")
  {
    eb->undoableSelectParameter({ 352, VoiceGroup::II });

    THEN("Single Preset Loaded")
    {
      eb->undoableLoad(presets.getSinglePreset());
      CHECK(eb->getSelected()->getID() == ParameterId { 158, VoiceGroup::I });
    }

    THEN("Split Preset Loaded")
    {
      eb->undoableLoad(presets.getSplitPreset());
      CHECK(eb->getSelected()->getID() == ParameterId { 158, VoiceGroup::II });
    }

    THEN("Split converted")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToDual(scope->getTransaction(), SoundType::Split);
      CHECK(eb->getSelected()->getID() == ParameterId { 158, VoiceGroup::II });
    }

    THEN("Single converted")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToSingle(scope->getTransaction(), VoiceGroup::II);
      CHECK(eb->getSelected()->getID() == ParameterId { 158, VoiceGroup::I });
    }
  }

  WHEN("FB: Effects - From x selected")
  {
    eb->undoableSelectParameter({ 354, VoiceGroup::II });

    THEN("Single Preset Loaded")
    {
      eb->undoableLoad(presets.getSinglePreset());
      CHECK(eb->getSelected()->getID() == ParameterId { 160, VoiceGroup::I });
    }

    THEN("Split Preset Loaded")
    {
      eb->undoableLoad(presets.getSplitPreset());
      CHECK(eb->getSelected()->getID() == ParameterId { 160, VoiceGroup::II });
    }

    THEN("Split converted")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToDual(scope->getTransaction(), SoundType::Split);
      CHECK(eb->getSelected()->getID() == ParameterId { 160, VoiceGroup::II });
    }

    THEN("Single converted")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToSingle(scope->getTransaction(), VoiceGroup::II);
      CHECK(eb->getSelected()->getID() == ParameterId { 160, VoiceGroup::I });
    }
  }

  WHEN("OUT: To FX x selected")
  {
    eb->undoableSelectParameter({ 362, VoiceGroup::II });

    THEN("Single Preset Loaded")
    {
      eb->undoableLoad(presets.getSinglePreset());
      CHECK(eb->getSelected()->getID() == ParameterId { 185, VoiceGroup::I });
    }

    THEN("Single converted")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToSingle(scope->getTransaction(), VoiceGroup::II);
      CHECK(eb->getSelected()->getID() == ParameterId { 185, VoiceGroup::I });
    }
  }

  WHEN("Part: Volume selected")
  {
    eb->undoableSelectParameter({ 358, VoiceGroup::II });

    THEN("Single Preset Loaded")
    {
      eb->undoableLoad(presets.getSinglePreset());
      CHECK(eb->getSelected()->getID() == ParameterId { 247, VoiceGroup::Global });
    }

    THEN("Single converted")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToSingle(scope->getTransaction(), VoiceGroup::II);
      CHECK(eb->getSelected()->getID() == ParameterId { 247, VoiceGroup::Global });
    }
  }

  WHEN("Part: Tune selected")
  {
    eb->undoableSelectParameter({ 360, VoiceGroup::II });

    THEN("Single Preset Loaded")
    {
      eb->undoableLoad(presets.getSinglePreset());
      CHECK(eb->getSelected()->getID() == ParameterId { 248, VoiceGroup::Global });
    }

    THEN("Single converted")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToSingle(scope->getTransaction(), VoiceGroup::II);
      CHECK(eb->getSelected()->getID() == ParameterId { 248, VoiceGroup::Global });
    }
  }

  WHEN("Part: Fade From selected")
  {
    eb->undoableSelectParameter({ 396, VoiceGroup::II });

    THEN("Split Preset Loaded")
    {
      eb->undoableLoad(presets.getSplitPreset());
      CHECK(eb->getSelected()->getID() == ParameterId({ 358, VoiceGroup::II }));
    }

    THEN("Single Preset Loaded")
    {
      eb->undoableLoad(presets.getSinglePreset());
      CHECK(eb->getSelected()->getID() == ParameterId({ 247, VoiceGroup::Global }));
    }

    THEN("Split converted")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToDual(scope->getTransaction(), SoundType::Split);
      CHECK(eb->getSelected()->getID() == ParameterId({ 358, VoiceGroup::II }));
    }

    THEN("Single converted")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToSingle(scope->getTransaction(), VoiceGroup::II);
      CHECK(eb->getSelected()->getID() == ParameterId({ 247, VoiceGroup::Global }));
    }
  }

  WHEN("Part: Fade Range selected")
  {
    eb->undoableSelectParameter({ 397, VoiceGroup::II });

    THEN("Split Preset Loaded")
    {
      eb->undoableLoad(presets.getSplitPreset());
      CHECK(eb->getSelected()->getID() == ParameterId({ 358, VoiceGroup::II }));
    }

    THEN("Single Preset Loaded")
    {
      eb->undoableLoad(presets.getSinglePreset());
      CHECK(eb->getSelected()->getID() == ParameterId({ 247, VoiceGroup::Global }));
    }

    THEN("Split Convert")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToDual(scope->getTransaction(), SoundType::Split);
      CHECK(eb->getSelected()->getID() == ParameterId({ 358, VoiceGroup::II }));
    }

    THEN("Single Convert")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToSingle(scope->getTransaction(), VoiceGroup::II);
      CHECK(eb->getSelected()->getID() == ParameterId({ 247, VoiceGroup::Global }));
    }
  }
}

TEST_CASE("Split Loaded")
{
  MockPresetStorage presets;

  auto eb = TestHelper::getEditBuffer();
  eb->undoableLoad(presets.getSplitPreset());

  Application::get().getHWUI()->setCurrentVoiceGroup(VoiceGroup::II);
  CHECK(eb->getType() == SoundType::Split);

  WHEN("OUT: To FX x selected")
  {
    eb->undoableSelectParameter({ 362, VoiceGroup::II });

    THEN("Single Preset Loaded")
    {
      eb->undoableLoad(presets.getSinglePreset());
      CHECK(eb->getSelected()->getID() == ParameterId { 185, VoiceGroup::I });
    }

    THEN("Single Convert")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToSingle(scope->getTransaction(), VoiceGroup::II);
      CHECK(eb->getSelected()->getID() == ParameterId { 185, VoiceGroup::I });
    }
  }

  WHEN("Part: Volume selected")
  {
    eb->undoableSelectParameter({ 358, VoiceGroup::II });

    THEN("Single Preset Loaded")
    {
      eb->undoableLoad(presets.getSinglePreset());
      CHECK(eb->getSelected()->getID() == ParameterId { 247, VoiceGroup::Global });
    }

    THEN("Single Convert")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToSingle(scope->getTransaction(), VoiceGroup::II);
      CHECK(eb->getSelected()->getID() == ParameterId { 247, VoiceGroup::Global });
    }
  }

  WHEN("Part: Tune selected")
  {
    eb->undoableSelectParameter({ 360, VoiceGroup::II });

    THEN("Single Preset Loaded")
    {
      eb->undoableLoad(presets.getSinglePreset());
      CHECK(eb->getSelected()->getID() == ParameterId { 248, VoiceGroup::Global });
    }

    THEN("Single Convert")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToSingle(scope->getTransaction(), VoiceGroup::II);
      CHECK(eb->getSelected()->getID() == ParameterId { 248, VoiceGroup::Global });
    }
  }

  WHEN("Split Point selected")
  {
    eb->undoableSelectParameter({ 356, VoiceGroup::Global });

    THEN("Single Preset Loaded")
    {
      eb->undoableLoad(presets.getSinglePreset());
      CHECK(eb->getSelected()->getID() == ParameterId { 247, VoiceGroup::Global });
    }

    THEN("Layer Preset Loaded")
    {
      eb->undoableLoad(presets.getLayerPreset());
      CHECK(eb->getSelected()->getID() == ParameterId { 358, VoiceGroup::II });
    }

    THEN("Layer Convert")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToDual(scope->getTransaction(), SoundType::Layer);
      CHECK(eb->getSelected()->getID() == ParameterId { 358, VoiceGroup::II });
    }

    THEN("Single Convert")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToSingle(scope->getTransaction(), VoiceGroup::II);
      CHECK(eb->getSelected()->getID() == ParameterId { 247, VoiceGroup::Global });
    }
  }
}
#include <testing/TestHelper.h>
#include <testing/unit-tests/mock/EditBufferNamedLogicalParts.h>
#include <testing/unit-tests/mock/MockPresetStorage.h>
#include <proxies/hwui/HWUI.h>

TEST_CASE("FROM Layer")
{
  MockPresetStorage presets;

  auto eb = TestHelper::getEditBuffer();
  EditBufferUseCases ebUseCases(eb);
  ebUseCases.undoableLoad(presets.getLayerPreset());

  Application::get().getHWUI()->setCurrentVoiceGroup(VoiceGroup::II);

  CHECK(eb->getType() == SoundType::Layer);

  WHEN("FB A/B x selected")
  {
    ebUseCases.selectParameter({ 346, VoiceGroup::II });

    THEN("Single Preset Loaded")
    {
      ebUseCases.undoableLoad(presets.getSinglePreset());
      CHECK(eb->getSelected(VoiceGroup::I)->getID() == ParameterId { 346, VoiceGroup::I });
    }

    THEN("Split Preset Loaded")
    {
      ebUseCases.undoableLoad(presets.getSplitPreset());
      CHECK(eb->getSelected(VoiceGroup::II)->getID() == ParameterId { 346, VoiceGroup::II });
    }

    THEN("Split converted")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToDual(scope->getTransaction(), SoundType::Split);
      CHECK(eb->getSelected(VoiceGroup::II)->getID() == ParameterId { 346, VoiceGroup::II });
    }

    THEN("Single converted")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToSingle(scope->getTransaction(), VoiceGroup::II);
      CHECK(eb->getSelected(VoiceGroup::I)->getID() == ParameterId { 346, VoiceGroup::I });
    }
  }

  WHEN("FB A/B x - A-B selected")
  {
    ebUseCases.selectParameter({ 348, VoiceGroup::II });

    THEN("Single Preset Loaded")
    {
      ebUseCases.undoableLoad(presets.getSinglePreset());
      CHECK(eb->getSelected(VoiceGroup::I)->getID() == ParameterId { 346, VoiceGroup::I });
    }

    THEN("Split Preset Loaded")
    {
      ebUseCases.undoableLoad(presets.getSplitPreset());
      CHECK(eb->getSelected(VoiceGroup::II)->getID() == ParameterId { 346, VoiceGroup::II });
    }

    THEN("Split converted")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToDual(scope->getTransaction(), SoundType::Split);
      CHECK(eb->getSelected(VoiceGroup::II)->getID() == ParameterId { 346, VoiceGroup::II });
    }

    THEN("Single converted")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToSingle(scope->getTransaction(), VoiceGroup::II);
      CHECK(eb->getSelected(VoiceGroup::II)->getID() == ParameterId { 346, VoiceGroup::I });
    }
  }

  WHEN("FB: Comb - From x selected")
  {
    ebUseCases.selectParameter({ 350, VoiceGroup::II });

    THEN("Single Preset Loaded")
    {
      ebUseCases.undoableLoad(presets.getSinglePreset());
      CHECK(eb->getSelected(VoiceGroup::I)->getID() == ParameterId { 156, VoiceGroup::I });
    }

    THEN("Split Preset Loaded")
    {
      ebUseCases.undoableLoad(presets.getSplitPreset());
      CHECK(eb->getSelected(VoiceGroup::II)->getID() == ParameterId { 156, VoiceGroup::II });
    }

    THEN("Split converted")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToDual(scope->getTransaction(), SoundType::Split);
      CHECK(eb->getSelected(VoiceGroup::II)->getID() == ParameterId { 156, VoiceGroup::II });
    }

    THEN("Single converted")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToSingle(scope->getTransaction(), VoiceGroup::II);
      CHECK(eb->getSelected(VoiceGroup::I)->getID() == ParameterId { 156, VoiceGroup::I });
    }
  }

  WHEN("FB: SV Filter - From x selected")
  {
    ebUseCases.selectParameter({ 352, VoiceGroup::II });

    THEN("Single Preset Loaded")
    {
      ebUseCases.undoableLoad(presets.getSinglePreset());
      CHECK(eb->getSelected(VoiceGroup::I)->getID() == ParameterId { 158, VoiceGroup::I });
    }

    THEN("Split Preset Loaded")
    {
      ebUseCases.undoableLoad(presets.getSplitPreset());
      CHECK(eb->getSelected(VoiceGroup::II)->getID() == ParameterId { 158, VoiceGroup::II });
    }

    THEN("Split converted")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToDual(scope->getTransaction(), SoundType::Split);
      CHECK(eb->getSelected(VoiceGroup::II)->getID() == ParameterId { 158, VoiceGroup::II });
    }

    THEN("Single converted")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToSingle(scope->getTransaction(), VoiceGroup::II);
      CHECK(eb->getSelected(VoiceGroup::I)->getID() == ParameterId { 158, VoiceGroup::I });
    }
  }

  WHEN("FB: Effects - From x selected")
  {
    ebUseCases.selectParameter({ 354, VoiceGroup::II });

    THEN("Single Preset Loaded")
    {
      ebUseCases.undoableLoad(presets.getSinglePreset());
      CHECK(eb->getSelected(VoiceGroup::I)->getID() == ParameterId { 160, VoiceGroup::I });
    }

    THEN("Split Preset Loaded")
    {
      ebUseCases.undoableLoad(presets.getSplitPreset());
      CHECK(eb->getSelected(VoiceGroup::II)->getID() == ParameterId { 160, VoiceGroup::II });
    }

    THEN("Split converted")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToDual(scope->getTransaction(), SoundType::Split);
      CHECK(eb->getSelected(VoiceGroup::II)->getID() == ParameterId { 160, VoiceGroup::II });
    }

    THEN("Single converted")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToSingle(scope->getTransaction(), VoiceGroup::II);
      CHECK(eb->getSelected(VoiceGroup::I)->getID() == ParameterId { 160, VoiceGroup::I });
    }
  }

  WHEN("OUT: To FX x selected")
  {
    ebUseCases.selectParameter({ 362, VoiceGroup::II });

    THEN("Single Preset Loaded")
    {
      ebUseCases.undoableLoad(presets.getSinglePreset());
      CHECK(eb->getSelected(VoiceGroup::I)->getID() == ParameterId { 185, VoiceGroup::I });
    }

    THEN("Single converted")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToSingle(scope->getTransaction(), VoiceGroup::II);
      CHECK(eb->getSelected(VoiceGroup::I)->getID() == ParameterId { 185, VoiceGroup::I });
    }
  }

  WHEN("Part: Volume selected")
  {
    ebUseCases.selectParameter({ 358, VoiceGroup::II });

    THEN("Single Preset Loaded")
    {
      ebUseCases.undoableLoad(presets.getSinglePreset());
      CHECK(eb->getSelected(VoiceGroup::Global)->getID() == ParameterId { 247, VoiceGroup::Global });
    }

    THEN("Single converted")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToSingle(scope->getTransaction(), VoiceGroup::II);
      CHECK(eb->getSelected(VoiceGroup::Global)->getID() == ParameterId { 247, VoiceGroup::Global });
    }
  }

  WHEN("Part: Tune selected")
  {
    ebUseCases.selectParameter({ 360, VoiceGroup::II });

    THEN("Single Preset Loaded")
    {
      ebUseCases.undoableLoad(presets.getSinglePreset());
      CHECK(eb->getSelected(VoiceGroup::Global)->getID() == ParameterId { 248, VoiceGroup::Global });
    }

    THEN("Single converted")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToSingle(scope->getTransaction(), VoiceGroup::II);
      CHECK(eb->getSelected(VoiceGroup::Global)->getID() == ParameterId { 248, VoiceGroup::Global });
    }
  }

  WHEN("Part: Fade From selected")
  {
    ebUseCases.selectParameter({ 396, VoiceGroup::II });

    THEN("Split Preset Loaded")
    {
      ebUseCases.undoableLoad(presets.getSplitPreset());
      CHECK(eb->getSelected(VoiceGroup::II)->getID() == ParameterId({ 358, VoiceGroup::II }));
    }

    THEN("Single Preset Loaded")
    {
      ebUseCases.undoableLoad(presets.getSinglePreset());
      CHECK(eb->getSelected(VoiceGroup::Global)->getID() == ParameterId({ 247, VoiceGroup::Global }));
    }

    THEN("Split converted")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToDual(scope->getTransaction(), SoundType::Split);
      CHECK(eb->getSelected(VoiceGroup::II)->getID() == ParameterId({ 358, VoiceGroup::II }));
    }

    THEN("Single converted")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToSingle(scope->getTransaction(), VoiceGroup::II);
      CHECK(eb->getSelected(VoiceGroup::Global)->getID() == ParameterId({ 247, VoiceGroup::Global }));
    }
  }

  WHEN("Part: Fade Range selected")
  {
    ebUseCases.selectParameter({ 397, VoiceGroup::II });

    THEN("Split Preset Loaded")
    {
      ebUseCases.undoableLoad(presets.getSplitPreset());
      CHECK(eb->getSelected(VoiceGroup::II)->getID() == ParameterId({ 358, VoiceGroup::II }));
    }

    THEN("Single Preset Loaded")
    {
      ebUseCases.undoableLoad(presets.getSinglePreset());
      CHECK(eb->getSelected(VoiceGroup::Global)->getID() == ParameterId({ 247, VoiceGroup::Global }));
    }

    THEN("Split Convert")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToDual(scope->getTransaction(), SoundType::Split);
      CHECK(eb->getSelected(VoiceGroup::II)->getID() == ParameterId({ 358, VoiceGroup::II }));
    }

    THEN("Single Convert")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToSingle(scope->getTransaction(), VoiceGroup::II);
      CHECK(eb->getSelected(VoiceGroup::Global)->getID() == ParameterId({ 247, VoiceGroup::Global }));
    }
  }
}

TEST_CASE("Split Loaded")
{
  MockPresetStorage presets;

  auto eb = TestHelper::getEditBuffer();
  EditBufferUseCases ebUseCases(eb);

  ebUseCases.undoableLoad(presets.getSplitPreset());

  Application::get().getHWUI()->setCurrentVoiceGroup(VoiceGroup::II);
  CHECK(eb->getType() == SoundType::Split);

  WHEN("OUT: To FX x selected")
  {
    ebUseCases.selectParameter({ 362, VoiceGroup::II });

    THEN("Single Preset Loaded")
    {
      ebUseCases.undoableLoad(presets.getSinglePreset());
      CHECK(eb->getSelected(VoiceGroup::I)->getID() == ParameterId { 185, VoiceGroup::I });
    }

    THEN("Single Convert")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToSingle(scope->getTransaction(), VoiceGroup::II);
      CHECK(eb->getSelected(VoiceGroup::I)->getID() == ParameterId { 185, VoiceGroup::I });
    }
  }

  WHEN("Part: Volume selected")
  {
    ebUseCases.selectParameter({ 358, VoiceGroup::II });

    THEN("Single Preset Loaded")
    {
      ebUseCases.undoableLoad(presets.getSinglePreset());
      CHECK(eb->getSelected(VoiceGroup::Global)->getID() == ParameterId { 247, VoiceGroup::Global });
    }

    THEN("Single Convert")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToSingle(scope->getTransaction(), VoiceGroup::II);
      CHECK(eb->getSelected(VoiceGroup::Global)->getID() == ParameterId { 247, VoiceGroup::Global });
    }
  }

  WHEN("Part: Tune selected")
  {
    ebUseCases.selectParameter({ 360, VoiceGroup::II });

    THEN("Single Preset Loaded")
    {
      ebUseCases.undoableLoad(presets.getSinglePreset());
      CHECK(eb->getSelected(VoiceGroup::Global)->getID() == ParameterId { 248, VoiceGroup::Global });
    }

    THEN("Single Convert")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToSingle(scope->getTransaction(), VoiceGroup::II);
      CHECK(eb->getSelected(VoiceGroup::Global)->getID() == ParameterId { 248, VoiceGroup::Global });
    }
  }

  WHEN("Split Point selected")
  {
    ebUseCases.selectParameter({ 356, VoiceGroup::I });

    THEN("Single Preset Loaded")
    {
      ebUseCases.undoableLoad(presets.getSinglePreset());
      CHECK(eb->getSelected(VoiceGroup::Global)->getID() == ParameterId { 247, VoiceGroup::Global });
    }

    THEN("Layer Preset Loaded")
    {
      ebUseCases.undoableLoad(presets.getLayerPreset());
      CHECK(eb->getSelected(VoiceGroup::II)->getID() == ParameterId { 358, VoiceGroup::II });
    }

    THEN("Layer Convert")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToDual(scope->getTransaction(), SoundType::Layer);
      CHECK(eb->getSelected(VoiceGroup::II)->getID() == ParameterId { 358, VoiceGroup::II });
    }

    THEN("Single Convert")
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToSingle(scope->getTransaction(), VoiceGroup::II);
      CHECK(eb->getSelected(VoiceGroup::Global)->getID() == ParameterId { 247, VoiceGroup::Global });
    }
  }
}
#include <testing/TestHelper.h>
#include <testing/unit-tests/mock/MockPresetStorage.h>
#include <presets/Preset.h>
#include <presets/PresetManager.h>

TEST_CASE("Convert Split to Layer With Part Names")
{
  auto eb = TestHelper::getEditBuffer();

  auto setVGName = [&](auto transaction, auto vg, auto name) { eb->setVoiceGroupName(transaction, name, vg); };

  {
    auto scope = TestHelper::createTestScope();
    auto trans = scope->getTransaction();

    THEN("Split -> Layer")
    {
      TestHelper::initDualEditBuffer<SoundType::Split>(trans);

      setVGName(trans, VoiceGroup::I, "I");
      setVGName(trans, VoiceGroup::II, "II");

      eb->undoableConvertToDual(trans, SoundType::Layer);

      CHECK(eb->getVoiceGroupName(VoiceGroup::I) == "I");
      CHECK(eb->getVoiceGroupName(VoiceGroup::II) == "II");
    }

    THEN("Layer -> Split")
    {
      TestHelper::initDualEditBuffer<SoundType::Layer>(trans);

      setVGName(trans, VoiceGroup::I, "I");
      setVGName(trans, VoiceGroup::II, "II");

      eb->undoableConvertToDual(trans, SoundType::Split);

      CHECK(eb->getVoiceGroupName(VoiceGroup::I) == "I");
      CHECK(eb->getVoiceGroupName(VoiceGroup::II) == "II");
    }
  }
}
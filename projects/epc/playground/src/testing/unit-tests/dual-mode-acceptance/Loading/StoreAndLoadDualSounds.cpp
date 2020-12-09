#include <testing/TestHelper.h>
#include <testing/unit-tests/mock/EditBufferNamedLogicalParts.h>
#include <testing/unit-tests/mock/MockPresetStorage.h>
#include <presets/Preset.h>
#include <presets/PresetParameter.h>

TEST_CASE("Store and Load Preset yields same editbuffer")
{
  MockPresetStorage presets;

  auto layerPreset = presets.getLayerPreset();

  using EBL = EditBufferLogicalParts;
  auto scope = TestHelper::createTestScope();
  auto transaction = scope->getTransaction();
  TestHelper::initDualEditBuffer<SoundType::Layer>(transaction);

  auto eb = TestHelper::getEditBuffer();

  WHEN("Unison Voices Changed")
  {
    auto voicesI = EBL::getUnisonVoice<VoiceGroup::I>();
    voicesI->setCPFromHwui(transaction, 1);  // 12 Voices

    THEN("Stored")
    {
      layerPreset->copyFrom(transaction, eb);

      WHEN("Loaded")
      {
        eb->undoableLoad(transaction, layerPreset, true);

        THEN("Unison Voices are 12")
        {
          CHECK(voicesI->getDisplayString() == "12 voices");
        }
      }
    }
  }
}
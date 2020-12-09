#include <testing/TestHelper.h>
#include <testing/unit-tests/mock/MockPresetStorage.h>
#include <presets/Preset.h>
#include <presets/PresetManager.h>

TEST_CASE("Store Presets With Part Names", "[Preset][Store]")
{
  auto eb = TestHelper::getEditBuffer();
  auto pm = TestHelper::getPresetManager();

  auto setVGName
      = [](auto trans, Preset* p, VoiceGroup vg, const char* n) { p->undoableSetVoiceGroupName(trans, vg, n); };

  MockPresetStorage presets;

  auto singlePreset = presets.getSinglePreset();
  auto layerPreset = presets.getLayerPreset();

  auto bank = static_cast<Bank*>(singlePreset->getParent());

  {
    auto scope = TestHelper::createTestScope();
    auto trans = scope->getTransaction();

    setVGName(trans, singlePreset, VoiceGroup::I, "I");
    setVGName(trans, singlePreset, VoiceGroup::II, "II");

    setVGName(trans, layerPreset, VoiceGroup::I, "I");
    setVGName(trans, layerPreset, VoiceGroup::II, "II");

    THEN("Load -> Store -> Load Single")
    {

      CHECK(singlePreset->getVoiceGroupName(VoiceGroup::I) == "I");
      CHECK(singlePreset->getVoiceGroupName(VoiceGroup::II) == "II");

      eb->undoableLoad(trans, singlePreset, true);

      CHECK(eb->getVoiceGroupName(VoiceGroup::I) == "I");
      CHECK(eb->getVoiceGroupName(VoiceGroup::II) == "II");

      auto storedPreset = bank->insertAndLoadPreset(scope->getTransaction(), 0, std::make_unique<Preset>(bank, *eb));

      CHECK(storedPreset->getUuid() != singlePreset->getUuid());

      CHECK(storedPreset->getVoiceGroupName(VoiceGroup::I) == "I");
      CHECK(storedPreset->getVoiceGroupName(VoiceGroup::II) == "II");

      eb->undoableLoad(trans, storedPreset, true);
      CHECK(eb->getVoiceGroupName(VoiceGroup::I) == "I");
      CHECK(eb->getVoiceGroupName(VoiceGroup::II) == "II");
    }

    THEN("Load -> Store -> Load Dual")
    {

      CHECK(layerPreset->getVoiceGroupName(VoiceGroup::I) == "I");
      CHECK(layerPreset->getVoiceGroupName(VoiceGroup::II) == "II");

      eb->undoableLoad(trans, layerPreset, true);

      CHECK(eb->getVoiceGroupName(VoiceGroup::I) == "I");
      CHECK(eb->getVoiceGroupName(VoiceGroup::II) == "II");

      auto storedPreset = bank->insertAndLoadPreset(scope->getTransaction(), 0, std::make_unique<Preset>(bank, *eb));

      CHECK(storedPreset->getUuid() != layerPreset->getUuid());

      CHECK(storedPreset->getVoiceGroupName(VoiceGroup::I) == "I");
      CHECK(storedPreset->getVoiceGroupName(VoiceGroup::II) == "II");

      eb->undoableLoad(trans, storedPreset, true);
      CHECK(eb->getVoiceGroupName(VoiceGroup::I) == "I");
      CHECK(eb->getVoiceGroupName(VoiceGroup::II) == "II");
    }
  }
}
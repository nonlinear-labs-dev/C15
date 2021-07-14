#include <testing/TestHelper.h>
#include <testing/unit-tests/mock/MockPresetStorage.h>
#include <presets/Preset.h>
#include <presets/PresetManager.h>
#include <xml/FileOutStream.h>
#include <xml/VersionAttribute.h>
#include <presets/BankActions.h>

TEST_CASE("Export/Import Presets With Part Names", "[Preset][Store][Export][Import]")
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

    auto CHECK_LABELS_PRESENT = [](const Preset* p) {
      CHECK(p->getVoiceGroupName(VoiceGroup::I) == "I");
      CHECK(p->getVoiceGroupName(VoiceGroup::II) == "II");
    };

    THEN("Export Bank -> Import Bank -> Check Labels")
    {
      CHECK_LABELS_PRESENT(singlePreset);
      CHECK_LABELS_PRESENT(layerPreset);

      auto oldNumPresets = bank->getNumPresets();
      auto oldBankName = bank->getName(false);

      {
        PresetBankSerializer serializer(bank, false);
        FileOutStream stream("/tmp/testbank.xml", false);
        XmlWriter writer(stream);
        serializer.write(writer, VersionAttribute::get());
      }

      auto newBank = [&] {
        FileInStream stream("/tmp/testbank.xml", false);
        auto& bankActions = Application::get().getPresetManager()->findActionManager<BankActions>();
        return bankActions.importBank(trans, stream, "0", "0", "/tmp/testbank.xml");
      }();

      CHECK_LABELS_PRESENT(newBank->getPresetAt(0));
      CHECK_LABELS_PRESENT(newBank->getPresetAt(1));

      CHECK(newBank->getNumPresets() == oldNumPresets);
      CHECK(newBank->getName(false) == oldBankName);
      pm->deleteBank(trans, newBank->getUuid());
    }
  }
}

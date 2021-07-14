#include <testing/TestHelper.h>
#include <testing/unit-tests/mock/FuxieSwarmsTestBank.h>
#include <presets/Preset.h>
#include <presets/PresetParameter.h>
#include <parameters/scale-converters/ScaleConverter.h>
#include <parameters/scale-converters/Linear12CountScaleConverter.h>
#include <parameters/scale-converters/LinearCountScaleConverter.h>
#include <parameters/scale-converters/dimension/VoicesDimension.h>
#include <http/UpdateDocumentMaster.h>
#include <xml/MemoryInStream.h>
#include <libundo/undo/Scope.h>
#include <parameter_declarations.h>
#include <parameters/scale-converters/EnvelopeAttackDecayTimeMSScaleConverter.h>

TEST_CASE("Import Bank Fresh Results in Correct Voices", "[Unison]")
{
  FuxieSwarmsTestBank bank;
  auto preset = bank.getPreset(0);
  auto eb = TestHelper::getEditBuffer();

  {
    auto scope = TestHelper::createTestScope();
    TestHelper::initSingleEditBuffer(scope->getTransaction());
  }

  WHEN("Unison Voices Correct")
  {
    PresetParameter* unisonVoices = nullptr;
    CHECK_NOTHROW(unisonVoices = preset->findParameterByID({ 249, VoiceGroup::I }, true));
    CHECK(unisonVoices);

    const auto ogValue = unisonVoices->getValue();
    auto voicesI = eb->findParameterByID({ 249, VoiceGroup::I });
    CHECK(voicesI->getValue().getDisplayString(ogValue) == "8 voices");
    CHECK(voicesI->getValue().getDisplayString() == "1 voice (off)");

    THEN("Load Preset -> voices == 8")
    {
      auto scope = TestHelper::createTestScope();
      auto transaction = scope->getTransaction();
      eb->undoableLoad(transaction, preset, true);

      CHECK(eb->getType() == SoundType::Single);
      CHECK(voicesI->getDisplayString() == "8 voices");
    }
  }
}

TEST_CASE("MC Smoothing A-D set to 20ms")
{
  FuxieSwarmsTestBank bank;
  auto preset = bank.getPreset(0);
  CHECK(preset != nullptr);

  auto mcASmoothing = preset->findParameterByID({ C15::PID::MC_Time_A, VoiceGroup::Global }, false);
  CHECK(mcASmoothing != nullptr);

  EnvelopeAttackDecayTimeMSScaleConverter conv;
  auto& dim = conv.getDimension();
  auto val = mcASmoothing->getValue();
  auto display = dim.stringize(conv.controlPositionToDisplay(val));
  CHECK(display == "20.0 ms");
}

TEST_CASE("UnisonVoices Conversion rules")
{
  class UpdateDocumentMasterMock : public UpdateDocumentMaster
  {
   public:
    void writeDocument(Writer&, tUpdateID) const override
    {
    }
  };

  UpdateDocumentMasterMock root;
  PresetManager pm(&root);
  Bank bank(&pm);

  auto readXml = [&](auto xml) {
    MemoryInStream stream(xml, false);
    UNDO::Scope scope(&root);
    auto transactionScope = scope.startTransaction("foo");
    XmlReader reader(stream, transactionScope->getTransaction());
    reader.read<PresetBankSerializer>(&bank);
  };

  auto createBankXml = [](auto fileVersion, auto type, auto grpId, auto paramId, auto paramVal) {
    return nltools::string::concat(
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><bank version=\"", fileVersion, "\">",
        "<preset-order><uuid>aaaaaaaa-aaaa-aaaa-aaaa-aaaaaaaaaaaa</uuid></preset-order><preset pos=\"0\"><type>", type,
        "</type><uuid>aaaaaaaa-aaaa-aaaa-aaaa-aaaaaaaaaaaa</uuid><parameter-groups><parameter-group id=\"", grpId,
        "\">", "<parameter id=\"", paramId, "\"><value>", paramVal,
        "</value></parameter></parameter-group></parameter-groups></preset></bank>");
  };

  const auto uuid = Uuid { "aaaaaaaa-aaaa-aaaa-aaaa-aaaaaaaaaaaa" };

  WHEN("reading version 5")
  {
    THEN("minimum is 0 => 1 voice")
    {
      readXml(createBankXml(5, "", "Unison", 249, 0));
      auto p = bank.findPreset(uuid)->findParameterByID({ 249, VoiceGroup::I }, true);
      CHECK(p->getValue() == 0);
    }

    THEN("maximum is 11/23 => 12 voices")
    {
      readXml(createBankXml(5, "", "Unison", 249, 1));
      auto p = bank.findPreset(uuid)->findParameterByID({ 249, VoiceGroup::I }, true);
      CHECK(p->getValue() == Approx(11 / 23.0));
    }
  }

  WHEN("reading version 7")
  {
    THEN("maximum for single preset is 23 => 24 voices")
    {
      readXml(createBankXml(7, "Single", "I-Unison", "I-249", 1));
      auto p = bank.findPreset(uuid)->findParameterByID({ 249, VoiceGroup::I }, true);
      CHECK(p->getValue() == 1.0);
    }

    THEN("maximum for split preset is 11 => 12 voices")
    {
      readXml(createBankXml(7, "Split", "I-Unison", "I-249", 1));
      auto p = bank.findPreset(uuid)->findParameterByID({ 249, VoiceGroup::I }, true);
      CHECK(p->getValue() == Approx(11 / 23.0));
    }

    THEN("maximum for dual preset is 11 => 12 voices")
    {
      readXml(createBankXml(7, "Layer", "I-Unison", "I-249", 1));
      auto p = bank.findPreset(uuid)->findParameterByID({ 249, VoiceGroup::I }, true);
      CHECK(p->getValue() == Approx(11 / 23.0));
    }
  }

  WHEN("reading version 8")
  {
    THEN("maximum for single preset is 23 => 24 voices")
    {
      readXml(createBankXml(8, "Single", "I-Unison", "I-249", 1));
      auto p = bank.findPreset(uuid)->findParameterByID({ 249, VoiceGroup::I }, true);
      CHECK(p->getValue() == 1.0);
    }

    THEN("maximum for split preset is 11 => 12 voices")
    {
      readXml(createBankXml(8, "Split", "I-Unison", "I-249", 11 / 23.0));
      auto p = bank.findPreset(uuid)->findParameterByID({ 249, VoiceGroup::I }, true);
      CHECK(p->getValue() == Approx(11 / 23.0));
    }

    THEN("maximum for dual preset is 11 => 12 voices")
    {
      readXml(createBankXml(8, "Layer", "I-Unison", "I-249", 11 / 23.0));
      auto p = bank.findPreset(uuid)->findParameterByID({ 249, VoiceGroup::I }, true);
      CHECK(p->getValue() == Approx(11 / 23.0));
    }
  }
}

#include <catch.hpp>
#include <synth/input/InputEventStage.h>
#include <synth/C15Synth.h>
#include <mock/MockDSPHosts.h>
#include <testing/TestHelper.h>
#include <mock/TCDHelpers.h>

MidiRuntimeOptions createTCDSettings()
{
  MidiRuntimeOptions options;
  using tMSG = nltools::msg::Setting::MidiSettingsMessage;
  tMSG msg;

  msg.receiveChannel = MidiReceiveChannel::Omni;
  msg.receiveSplitChannel = MidiReceiveChannelSplit::Omni;

  msg.sendChannel = MidiSendChannel::CH_1;
  msg.sendSplitChannel = MidiSendChannelSplit::CH_1;

  msg.routings = TestHelper::createFullMappings(true);

  msg.bendercc = BenderCC::Pitchbend;
  msg.aftertouchcc = AftertouchCC::ChannelPressure;
  msg.pedal1cc = PedalCC::CC20;
  msg.pedal2cc = PedalCC::CC21;
  msg.pedal3cc = PedalCC::CC22;
  msg.pedal4cc = PedalCC::CC23;
  msg.ribbon1cc = RibbonCC::CC24;
  msg.ribbon2cc = RibbonCC::CC25;

  options.update(msg);
  return options;
}

class MockTCDDecoder : public TCDDecoder
{
 public:
  using TCDDecoder::TCDDecoder;
  void setValue(float v)
  {
    value = v;
  }

  void setType(DecoderEventType type)
  {
    m_type = type;
  }

  void setKeyOrCtrl(int k)
  {
    keyOrController = k;
  }
};

TEST_CASE("TCD Decoder Reset", "[TCD]")
{
  MockDSPHost host;
  auto setting = createTCDSettings();
  KeyShift shift;
  MockTCDDecoder decoder(&host, &setting, &shift);

  decoder.setKeyOrCtrl(12);
  decoder.setType(DecoderEventType::KeyUp);
  decoder.setValue(187);

  decoder.reset();
  CHECK(decoder.getValue() == 0);
  CHECK(decoder.getEventType() == DecoderEventType::UNKNOWN);
  CHECK(decoder.getKeyOrController() == -1);
}

TEST_CASE("TCD in leads to key down and send midi", "[MIDI][TCD]")
{
  std::vector<nltools::msg::Midi::SimpleMessage> sendMessages;
  PassOnKeyDownHost dsp { 17, 1.0, VoiceGroup::I };
  auto settings = createTCDSettings();
  InputEventStage eventStage { &dsp, &settings, [] {},
                               [&](nltools::msg::Midi::SimpleMessage msg) { sendMessages.push_back(msg); },
                               [](auto) {} };

  WHEN("Keypos and KeyDown is received")
  {
    eventStage.onTCDMessage(TCD_HELPER::createKeyPosEvent(17));
    eventStage.onTCDMessage(TCD_HELPER::createKeyDownEvent(127, 127));

    THEN("DSP got notified")
    {
      CHECK(dsp.didReceiveKeyDown());
    }

    THEN("MIDI got send")
    {
      REQUIRE(sendMessages.size() == 4);
      CHECK(sendMessages[0].rawBytes[0] == 0xB0);
      CHECK(sendMessages[0].rawBytes[1] == 88);
      CHECK(sendMessages[0].rawBytes[2] == 0);

      CHECK(sendMessages[1].rawBytes[0] == 0x90);
      CHECK(sendMessages[1].rawBytes[1] == 17);
      CHECK(sendMessages[1].rawBytes[2] == 127);

      CHECK(sendMessages[2].rawBytes[0] == 0xB0);
      CHECK(sendMessages[2].rawBytes[1] == 88);
      CHECK(sendMessages[2].rawBytes[2] == 0);

      CHECK(sendMessages[3].rawBytes[0] == 0x90);
      CHECK(sendMessages[3].rawBytes[1] == 17);
      CHECK(sendMessages[3].rawBytes[2] == 127);
    }
  }
}

TEST_CASE("TCD in leads to key up and send midi", "[MIDI][TCD]")
{
  std::vector<nltools::msg::Midi::SimpleMessage> sendMessages;
  PassOnKeyUpHost dsp { 17, 1.0, VoiceGroup::I };
  auto settings = createTCDSettings();
  InputEventStage eventStage { &dsp, &settings, [] {},
                               [&](nltools::msg::Midi::SimpleMessage msg) { sendMessages.push_back(msg); },
                               [](auto) {} };

  WHEN("Keypos and KeyUp is received")
  {
    eventStage.onTCDMessage(TCD_HELPER::createKeyPosEvent(17));
    eventStage.onTCDMessage(TCD_HELPER::createKeyUpEvent(127, 127));

    THEN("DSP got notified")
    {
      CHECK(dsp.didReceiveKeyUp());
    }

    THEN("MIDI got send")
    {
      REQUIRE(sendMessages.size() == 4);
      CHECK(sendMessages[0].rawBytes[0] == 0xB0);
      CHECK(sendMessages[0].rawBytes[1] == 88);
      CHECK(sendMessages[0].rawBytes[2] == 0);

      CHECK(sendMessages[1].rawBytes[0] == 0x80);
      CHECK(sendMessages[1].rawBytes[1] == 17);
      CHECK(sendMessages[1].rawBytes[2] == 127);

      CHECK(sendMessages[2].rawBytes[0] == 0xB0);
      CHECK(sendMessages[2].rawBytes[1] == 88);
      CHECK(sendMessages[2].rawBytes[2] == 0);

      CHECK(sendMessages[3].rawBytes[0] == 0x80);
      CHECK(sendMessages[3].rawBytes[1] == 17);
      CHECK(sendMessages[3].rawBytes[2] == 127);
    }
  }
}

constexpr static uint8_t Pedal1 = 0b00000000;
constexpr static uint8_t Pedal2 = 0b00000001;
constexpr static uint8_t Pedal3 = 0b00000010;
constexpr static uint8_t Pedal4 = 0b00000011;
constexpr static uint8_t Bender = 0b00000100;
constexpr static uint8_t Aftertouch = 0b00000101;
constexpr static uint8_t Ribbon1 = 0b00000110;
constexpr static uint8_t Ribbon2 = 0b00000111;

TEST_CASE("TCD in leads to HW Change and send midi", "[MIDI][TCD]")
{
  std::vector<nltools::msg::Midi::SimpleMessage> sendMessages;
  PassOnHWReceived dsp { Pedal1, 1.0 };
  dsp.setType(SoundType::Single);
  auto settings = createTCDSettings();
  InputEventStage eventStage { &dsp, &settings, [] {},
                               [&](nltools::msg::Midi::SimpleMessage msg) { sendMessages.push_back(msg); },
                               [](auto) {} };
  const auto sixteenThousand = 0b11111010000000;

  WHEN("HW Change Received")
  {

    THEN("DSP got notified")
    {
      eventStage.onTCDMessage(TCD_HELPER::createFullPressureHWEvent(TCD_HELPER::TCD_HW_IDS::Pedal1));
      CHECK(dsp.didReceiveHW());
    }

    WHEN("Pedal Mapping is None")
    {
      settings.setPedal1(PedalCC::None);
      settings.set14BitSupportEnabled(true);

      WHEN("Pedal value is received from Internal")
      {
        eventStage.onTCDMessage(TCD_HELPER::createFullPressureHWEvent(TCD_HELPER::TCD_HW_IDS::Pedal1));
        THEN("No midi got send")
        {
          CHECK(sendMessages.empty());
        }
      }
    }

    WHEN("Bender Mapping is None")
    {
      settings.setBenderCC(BenderCC::None);
      settings.set14BitSupportEnabled(true);
      dsp.setExpectedHW(4);

      WHEN("Bender value is received from Internal")
      {
        eventStage.onTCDMessage(TCD_HELPER::createFullPressureHWEvent(TCD_HELPER::TCD_HW_IDS::Bender));
        THEN("No midi got send")
        {
          CHECK(sendMessages.empty());
        }
      }
    }

    WHEN("Ribbon Mapping is None")
    {
      settings.setRibbon1(RibbonCC::None);
      settings.set14BitSupportEnabled(true);
      dsp.setExpectedHW(6);

      WHEN("Ribbon value is received from Internal")
      {
        eventStage.onTCDMessage(TCD_HELPER::createFullPressureHWEvent(TCD_HELPER::TCD_HW_IDS::Ribbon1));
        THEN("No midi got send")
        {
          CHECK(sendMessages.empty());
        }
      }
    }

    WHEN("Aftertouch Mapping is None")
    {
      settings.setAftertouchCC(AftertouchCC::None);
      settings.set14BitSupportEnabled(true);
      dsp.setExpectedHW(5);

      WHEN("Aftertouch value is received from Internal")
      {
        eventStage.onTCDMessage(TCD_HELPER::createFullPressureHWEvent(TCD_HELPER::TCD_HW_IDS::Aftertouch));
        THEN("No midi got send")
        {
          CHECK(sendMessages.empty());
        }
      }
    }

    WHEN("CC01 and CC33 and 14 Bit support Enabled")
    {
      settings.setPedal1(PedalCC::CC01);
      settings.setSendSplitChannel(MidiSendChannelSplit::CH_2);
      settings.set14BitSupportEnabled(true);
      eventStage.onTCDMessage(TCD_HELPER::createFullPressureHWEvent(TCD_HELPER::TCD_HW_IDS::Pedal1));

      THEN("MIDI got send")
      {
        REQUIRE(sendMessages.size() == 2);
        CHECK(sendMessages[0].rawBytes[0] == 0xB0);
        CHECK(sendMessages[0].rawBytes[1] == 33);
        CHECK(sendMessages[0].rawBytes[2] == 0);

        CHECK(sendMessages[1].rawBytes[0] == 0xB0);
        CHECK(sendMessages[1].rawBytes[1] == 1);
        CHECK(sendMessages[1].rawBytes[2] == 127);
      }
    }

    WHEN("CC01 and CC33 and 14 Bit support Disabled")
    {
      settings.setPedal1(PedalCC::CC01);
      settings.setSendSplitChannel(MidiSendChannelSplit::CH_2);
      settings.set14BitSupportEnabled(false);
      eventStage.onTCDMessage(TCD_HELPER::createFullPressureHWEvent(TCD_HELPER::TCD_HW_IDS::Pedal1));

      THEN("MIDI got send")
      {
        REQUIRE(sendMessages.size() == 1);

        CHECK(sendMessages[0].rawBytes[0] == 0xB0);
        CHECK(sendMessages[0].rawBytes[1] == 1);
        CHECK(sendMessages[0].rawBytes[2] == 127);
      }
    }

    WHEN("CC02 and CC34")
    {
      settings.setPedal1(PedalCC::CC02);
      settings.setSendSplitChannel(MidiSendChannelSplit::CH_2);
      eventStage.onTCDMessage(TCD_HELPER::createFullPressureHWEvent(TCD_HELPER::TCD_HW_IDS::Pedal1));

      THEN("MIDI got send")
      {
        REQUIRE(sendMessages.size() == 2);
        CHECK(sendMessages[0].rawBytes[0] == 0xB0);
        CHECK(sendMessages[0].rawBytes[1] == 34);
        CHECK(sendMessages[0].rawBytes[2] == 0);

        CHECK(sendMessages[1].rawBytes[0] == 0xB0);
        CHECK(sendMessages[1].rawBytes[1] == 2);
        CHECK(sendMessages[1].rawBytes[2] == 127);
      }
    }
  }
}
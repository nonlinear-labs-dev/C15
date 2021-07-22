#include <catch.hpp>
#include <mock/InputEventStageTester.h>
#include <mock/MockDSPHosts.h>
#include <testing/TestHelper.h>

TEST_CASE("Aftertouch Mappings", "[MIDI][TCD]")
{
  constexpr static uint8_t BASE_TCD = 0b11100000;
  constexpr static uint8_t Aftertouch = 0b00000101;
  constexpr static auto sixteenThousand = 0b11111010000000;

  constexpr MidiEvent fullPressureTCDEvent
      = { BASE_TCD | Aftertouch, (uint8_t)(sixteenThousand >> 7), (uint8_t)(sixteenThousand & 127) };

  bool receivedHW = false;
  ConfigureableDSPHost host {};
  host.setType(SoundType::Single);
  host.setOnHWChangedCB([&](int hwID, float hwPos, auto) {
    CHECK(hwID == 5);
    CHECK(hwPos == 1.0f);
    receivedHW = true;
  });

  std::vector<nltools::msg::Midi::SimpleMessage> sendMidiMessages;
  MidiRuntimeOptions settings;
  InputEventStage eventStage(
      &host, &settings, [] {}, [&](auto msg) { sendMidiMessages.push_back(msg); }, [](auto) {});

  //set settings to not interfere with CC01
  {
    nltools::msg::Setting::MidiSettingsMessage msg;
    msg.receiveChannel = MidiReceiveChannel::CH_1;
    msg.sendChannel = MidiSendChannel::CH_1;
    msg.sendSplitChannel = MidiSendChannelSplit::None;

    msg.routings = TestHelper::createFullMappings(true);

    msg.pedal1cc = PedalCC::CC02;
    msg.pedal2cc = PedalCC::CC02;
    msg.pedal3cc = PedalCC::CC02;
    msg.pedal4cc = PedalCC::CC02;
    msg.ribbon1cc = RibbonCC::CC02;
    msg.ribbon2cc = RibbonCC::CC02;
    msg.bendercc = BenderCC::CC02;
    settings.update(msg);
  }

  WHEN("Mapped to Channel Pressure")
  {
    settings.setAftertouchCC(AftertouchCC::ChannelPressure);

    WHEN("Send MIDI Channel Pressure")
    {
      eventStage.onMIDIMessage({ 0xD0, 127, 127 });
      CHECK(receivedHW);
    }

    WHEN("Send CC01")
    {
      eventStage.onMIDIMessage({ 0xB0, 0x01, 127 });
      CHECK_FALSE(receivedHW);
    }
  }

  WHEN("Mapped to CC01")
  {
    settings.setAftertouchCC(AftertouchCC::CC01);

    WHEN("Send MIDI Channel Pressure")
    {
      eventStage.onMIDIMessage({ 0xD0, 127, 127 });
      CHECK_FALSE(receivedHW);
    }

    WHEN("Send CC01")
    {
      eventStage.onMIDIMessage({ 0xB0, 0x01, 127 });
      CHECK(receivedHW);
    }
  }

  WHEN("Mapped to Special Case PitchbendDown")
  {
    settings.setAftertouchCC(AftertouchCC::PitchbendDown);
    host.setOnHWChangedCB([&](int hwID, float hwPos, auto) {
      CHECK(hwID == 5);
      CHECK(hwPos == 1.0f);
      receivedHW = true;
    });

    WHEN("Send MIDI Pitchbend, -1.0")
    {
      eventStage.onMIDIMessage({ 0xE0, 0, 0 });
      CHECK(receivedHW);
    }

    WHEN("Send CC01")
    {
      eventStage.onMIDIMessage({ 0xB0, 0x01, 127 });
      CHECK_FALSE(receivedHW);
    }

    WHEN("receive TCD aftertouch, send as MIDI Channel Pressure -> \"-1\"")
    {
      eventStage.onTCDMessage(fullPressureTCDEvent);
      CHECK(receivedHW);
      REQUIRE(sendMidiMessages.size() == 1);
      CHECK(sendMidiMessages[0].numBytesUsed == 3);
      CHECK(sendMidiMessages[0].rawBytes[0] == 0xE0);
      CHECK(sendMidiMessages[0].rawBytes[1] == 0);
      CHECK(sendMidiMessages[0].rawBytes[2] == 0);
    }
  }

  WHEN("Mapped to Special Case PitchbendUp")
  {
    settings.setAftertouchCC(AftertouchCC::PitchbendUp);
    host.setType(SoundType::Split);
    host.setOnHWChangedCB([&](int hwID, float hwPos, auto) {
      CHECK(hwID == 5);
      CHECK(hwPos == 1.0f);
      receivedHW = true;
    });

    WHEN("Send MIDI Channel Pitchbend")
    {
      eventStage.onMIDIMessage({ 0xE0, 127, 127 });
      CHECK(receivedHW);
    }

    WHEN("Send CC01")
    {
      eventStage.onMIDIMessage({ 0xB0, 0x01, 127 });
      CHECK_FALSE(receivedHW);
    }

    WHEN("receive TCD aftertouch, send as MIDI Channel Pressure -> 1")
    {
      WHEN("Send Split Channel is None")
      {
        settings.setSendSplitChannel(MidiSendChannelSplit::None);
        eventStage.onTCDMessage(fullPressureTCDEvent);

        CHECK(receivedHW);
        REQUIRE(sendMidiMessages.size() == 1);
        CHECK(sendMidiMessages[0].numBytesUsed == 3);
        CHECK(sendMidiMessages[0].rawBytes[0] == 0xE0);
        CHECK(sendMidiMessages[0].rawBytes[1] == 127);
        CHECK(sendMidiMessages[0].rawBytes[2] == 127);
      }

      WHEN("Send Split Channel is Different from Send Channel")
      {
        settings.setSendChannel(MidiSendChannel::CH_1);
        settings.setSendSplitChannel(MidiSendChannelSplit::CH_2);

        eventStage.onTCDMessage(fullPressureTCDEvent);

        CHECK(receivedHW);
        REQUIRE(sendMidiMessages.size() == 2);
        CHECK(sendMidiMessages[0].numBytesUsed == 3);
        CHECK(sendMidiMessages[0].rawBytes[0] == 0xE0);
        CHECK(sendMidiMessages[0].rawBytes[1] == 127);
        CHECK(sendMidiMessages[0].rawBytes[2] == 127);
        CHECK(sendMidiMessages[1].numBytesUsed == 3);
        CHECK(sendMidiMessages[1].rawBytes[0] == 0xE1);
        CHECK(sendMidiMessages[1].rawBytes[1] == 127);
        CHECK(sendMidiMessages[1].rawBytes[2] == 127);
      }
    }
  }
}
#include <catch.hpp>
#include <mock/InputEventStageTester.h>
#include <mock/MockDSPHosts.h>

TEST_CASE("Pitchbend Mappings", "[MIDI][TCD]")
{
  bool receivedHW = false;
  ConfigureableDSPHost host {};
  host.setType(SoundType::Single);
  host.setOnHWChangedCB(
      [&](int hwID, float hwPos)
      {
        CHECK(hwID == 4);
        CHECK(hwPos == 1.0f);
        receivedHW = true;
      });

  std::vector<nltools::msg::Midi::SimpleMessage> sendMidiMessages;
  MidiRuntimeOptions settings;
  InputEventStage eventStage(&host, &settings, [&](auto msg) { sendMidiMessages.push_back(msg); });

  //set settings to not interfere with CC01
  {
    nltools::msg::Setting::MidiSettingsMessage msg;
    msg.receiveControllers = true;
    msg.sendControllers = true;
    msg.receiveChannel = MidiReceiveChannel::CH_1;
    msg.sendChannel = MidiSendChannel::CH_1;
    msg.pedal1cc = PedalCC::CC02;
    msg.pedal2cc = PedalCC::CC02;
    msg.pedal3cc = PedalCC::CC02;
    msg.pedal4cc = PedalCC::CC02;
    msg.ribbon1cc = RibbonCC::CC02;
    msg.ribbon2cc = RibbonCC::CC02;
    msg.aftertouchcc = AftertouchCC::CC02;
    settings.update(msg);
  }

  WHEN("Mapped to Channel Pitchbend")
  {
    settings.setBenderCC(BenderCC::Pitchbend);

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
  }

  WHEN("Mapped to CC01")
  {
    settings.setBenderCC(BenderCC::CC01);

    WHEN("Send MIDI Channel Pitchbend")
    {
      eventStage.onMIDIMessage({ 0xE0, 127, 127 });
      CHECK_FALSE(receivedHW);
    }

    WHEN("Send CC01")
    {
      eventStage.onMIDIMessage({ 0xB0, 0x01, 127 });
      CHECK(receivedHW);
    }
  }
}
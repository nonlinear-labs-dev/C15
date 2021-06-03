#include <testing/TestHelper.h>
#include <Types.h>
#include <mock/MockDSPHosts.h>

//This ensures that this ticket is fixed:

TEST_CASE("Send HW-Change only in Split Sound on Split Channel")
{
  constexpr static uint8_t BASE_TCD = 0b11100000;
  constexpr static uint8_t Aftertouch = 0b00000101;
  constexpr static auto sixteenThousand = 0b11111010000000;

  constexpr MidiEvent fullPressureTCDEvent
      = { BASE_TCD | Aftertouch, (uint8_t) (sixteenThousand >> 7), (uint8_t) (sixteenThousand & 127) };

  ConfigureableDSPHost host {};
  host.setType(SoundType::Single);

  std::vector<nltools::msg::Midi::SimpleMessage> sendMidiMessages;
  MidiRuntimeOptions settings;
  InputEventStage eventStage(
      &host, &settings, [] {}, [&](auto msg) { sendMidiMessages.push_back(msg); });

  //set settings to not interfere with CC01
  {
    nltools::msg::Setting::MidiSettingsMessage msg;
    msg.receiveControllers = true;
    msg.sendControllers = true;
    msg.receiveChannel = MidiReceiveChannel::CH_1;
    msg.sendChannel = MidiSendChannel::CH_1;
    msg.sendSplitChannel = MidiSendChannelSplit::CH_2;
    msg.localControllers = true;
    msg.highResCCEnabled = false;
    msg.pedal1cc = PedalCC::CC02;
    msg.pedal2cc = PedalCC::CC02;
    msg.pedal3cc = PedalCC::CC02;
    msg.pedal4cc = PedalCC::CC02;
    msg.ribbon1cc = RibbonCC::CC02;
    msg.ribbon2cc = RibbonCC::CC02;
    msg.bendercc = BenderCC::CC02;
    msg.aftertouchcc = AftertouchCC::CC30;
    settings.update(msg);
  }

  auto sendTCDHWChange = [&]() { eventStage.onTCDMessage(fullPressureTCDEvent); };

  auto doTests = [&]()
  {
    WHEN("Sound is Single")
    {
      host.setType(SoundType::Single);

      WHEN("TCD HW Change is received")
      {
        sendTCDHWChange();

        THEN("HW Change got send on Primary Channel")
        {
          CHECK(sendMidiMessages.size() == 1);
        }
      }
    }

    WHEN("Sound is Layer")
    {
      host.setType(SoundType::Layer);

      WHEN("TCD HW Change is received")
      {
        sendTCDHWChange();

        THEN("HW Change got send on Primary Channel")
        {
          CHECK(sendMidiMessages.size() == 1);
        }
      }
    }

    WHEN("Sound is Split")
    {
      host.setType(SoundType::Split);

      WHEN("TCD HW Change is received")
      {
        sendTCDHWChange();

        THEN("HW Change got send on Primary Channel")
        {
          CHECK(sendMidiMessages.size() == 1);
        }
      }
    }
  };

  std::map<SoundType, int> expected = { { SoundType::Single, 1 }, { SoundType::Layer, 1 }, { SoundType::Split, 1 } };

  WHEN("Split Channel is Channel 2")
  {
    settings.setSendSplitChannel(MidiSendChannelSplit::CH_2);
    doTests();
  }

  WHEN("Split Channel is NONE")
  {
    settings.setSendSplitChannel(MidiSendChannelSplit::None);
    doTests();
  }

  WHEN("Split Channel is Common")
  {
    settings.setSendSplitChannel(MidiSendChannelSplit::Common);
    doTests();
  }
}
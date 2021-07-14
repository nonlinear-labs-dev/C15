#include <testing/TestHelper.h>
#include <mock/MockDSPHosts.h>
#include <mock/InputEventStageTester.h>

TEST_CASE("HW Source Enable Tests")
{
  //Setup
  ConfigureableDSPHost dsp {};
  dsp.setType(SoundType::Split);

  MidiRuntimeOptions options {};

  std::vector<InputEventStage::MIDIOutType> sendMidiMessages;
  InputEventStage eS(
      &dsp, &options,
      []() {

      },
      [&](InputEventStage::MIDIOutType m) { sendMidiMessages.emplace_back(m); });

  InputEventStageTester eventStage(&eS);

  std::vector<std::pair<uint32_t, float>> receivedHWMsg;
  dsp.setOnHWChangedCB([&](auto id, auto pos, auto) { receivedHWMsg.emplace_back(std::make_pair(id, pos)); });

  //prepare default settings

  auto initSettings = [&](const std::vector<nltools::msg::Setting::MidiSettingsMessage::MAPPING_INDEX>& aspects)
  {
    nltools::msg::Setting::MidiSettingsMessage msg;
    msg.pedal1cc = PedalCC::CC01;
    msg.pedal2cc = PedalCC::CC02;
    msg.pedal3cc = PedalCC::CC03;
    msg.pedal4cc = PedalCC::CC04;
    msg.aftertouchcc = AftertouchCC::CC05;
    msg.bendercc = BenderCC::CC06;
    msg.ribbon1cc = RibbonCC::CC07;
    msg.ribbon2cc = RibbonCC::CC08;

    msg.receiveChannel = MidiReceiveChannel::CH_1;
    msg.receiveSplitChannel = MidiReceiveChannelSplit::CH_2;
    msg.sendChannel = MidiSendChannel::CH_1;
    msg.sendSplitChannel = MidiSendChannelSplit::CH_2;

    msg.highVeloCCEnabled = false;
    msg.highResCCEnabled = false;

    msg.hwMappings = TestHelper::createFullMappings(false);

    for(auto aspect : aspects)
    {
      TestHelper::updateMappings(msg.hwMappings, aspect, true);
    }

    options.update(msg);
  };

  constexpr static uint8_t BASE_TCD = 0b11100000;
  constexpr static uint8_t Pedal1 = 0;
  constexpr static auto sixteenThousand = 0b11111010000000;

  constexpr MidiEvent fullPressureTCDEvent
      = { BASE_TCD | Pedal1, (uint8_t) (sixteenThousand >> 7), (uint8_t) (sixteenThousand & 127) };

  constexpr MidiEvent pedal1PressureMIDIEvent = { 0xB0, 1, 69 };
  constexpr MidiEvent pedal1PressureMIDIEvent_split = { 0xB1, 1, 69 };

  typedef nltools::msg::Setting::MidiSettingsMessage::MAPPING_INDEX tMAPPING_INDEX;

  WHEN("none")
  {
    initSettings({});

    bool didReceive = false;
    dsp.setOnHWChangedCB([&didReceive](auto, auto, auto) { didReceive = true; });

    eventStage.onTCDMessage(fullPressureTCDEvent);
    eventStage.onMIDIMessage(pedal1PressureMIDIEvent);

    CHECK(sendMidiMessages.empty());
    CHECK(!didReceive);
  }

  WHEN("local")
  {
    initSettings({ tMAPPING_INDEX::LOCAL });

    bool didReceive = false;
    dsp.setOnHWChangedCB([&didReceive](auto, auto, auto) { didReceive = true; });

    eventStage.onTCDMessage(fullPressureTCDEvent);
    CHECK(sendMidiMessages.empty());
    CHECK(didReceive);
  }

  WHEN("receive only primary")
  {
    initSettings({ tMAPPING_INDEX::RECEIVE_PRIMARY });

    bool didReceive = false;
    dsp.setOnHWChangedCB(
        [&didReceive](auto hwID, auto, auto)
        {
          didReceive = true;
          CHECK(hwID == 0);
        });

    eventStage.onMIDIMessage(pedal1PressureMIDIEvent);

    CHECK(didReceive);
    CHECK(sendMidiMessages.empty());
  }

  WHEN("receive local and send primary")
  {
    initSettings({ tMAPPING_INDEX::LOCAL, tMAPPING_INDEX::SEND_PRIMARY });

    bool didReceive = false;
    dsp.setOnHWChangedCB(
        [&didReceive](auto hwID, auto, auto)
        {
          didReceive = true;
          CHECK(hwID == 0);
        });

    eventStage.onTCDMessage(fullPressureTCDEvent);

    CHECK(didReceive);
    CHECK(!sendMidiMessages.empty());
  }

  WHEN("receive local and send split")
  {
    initSettings({ tMAPPING_INDEX::LOCAL, tMAPPING_INDEX::SEND_SPLIT });

    bool didReceive = false;
    dsp.setOnHWChangedCB(
        [&didReceive](auto hwID, auto, auto)
        {
          didReceive = true;
          CHECK(hwID == 0);
        });

    WHEN("is Split")
    {
      dsp.setType(SoundType::Split);

      eventStage.onTCDMessage(fullPressureTCDEvent);

      CHECK(didReceive);
      CHECK(!sendMidiMessages.empty());
      CHECK(sendMidiMessages.size() == 1);
    }

    WHEN("is not Split")
    {
      dsp.setType(SoundType::Single);

      eventStage.onTCDMessage(fullPressureTCDEvent);

      CHECK(didReceive);
      CHECK(sendMidiMessages.empty());
    }
  }

  WHEN("receive only split")
  {
    initSettings({ tMAPPING_INDEX::RECEIVE_SPLIT });

    bool didReceive = false;
    dsp.setOnHWChangedCB(
        [&didReceive](auto hwID, auto, auto)
        {
          didReceive = true;
          CHECK(hwID == 0);
        });

    WHEN("is Split")
    {
      dsp.setType(SoundType::Split);
      eventStage.onMIDIMessage(pedal1PressureMIDIEvent_split);

      CHECK(didReceive);
      CHECK(sendMidiMessages.empty());
    }

    WHEN("is Single")
    {
      dsp.setType(SoundType::Single);
      eventStage.onMIDIMessage(pedal1PressureMIDIEvent_split);

      CHECK(!didReceive);
      CHECK(sendMidiMessages.empty());
    }
  }
}

TEST_CASE("Aftertouch & Bender Enable/Disable Tests")
{
  //Setup
  ConfigureableDSPHost dsp {};
  dsp.setType(SoundType::Split);

  MidiRuntimeOptions options {};

  std::vector<InputEventStage::MIDIOutType> sendMidiMessages;
  InputEventStage eS(
      &dsp, &options,
      []() {

      },
      [&](InputEventStage::MIDIOutType m) { sendMidiMessages.emplace_back(m); });

  InputEventStageTester eventStage(&eS);

  std::vector<std::pair<uint32_t, float>> receivedHWMsg;
  dsp.setOnHWChangedCB([&](auto id, auto pos, auto) { receivedHWMsg.emplace_back(std::make_pair(id, pos)); });

  //prepare default settings

  auto initSettings = [&](const std::vector<nltools::msg::Setting::MidiSettingsMessage::HW_INDEX>& hws,
                          const std::vector<nltools::msg::Setting::MidiSettingsMessage::MAPPING_INDEX>& aspects)
  {
    nltools::msg::Setting::MidiSettingsMessage msg;
    msg.pedal1cc = PedalCC::CC01;
    msg.pedal2cc = PedalCC::CC02;
    msg.pedal3cc = PedalCC::CC03;
    msg.pedal4cc = PedalCC::CC04;
    msg.aftertouchcc = AftertouchCC::ChannelPressure;
    msg.bendercc = BenderCC::Pitchbend;
    msg.ribbon1cc = RibbonCC::CC07;
    msg.ribbon2cc = RibbonCC::CC08;

    msg.receiveChannel = MidiReceiveChannel::CH_1;
    msg.receiveSplitChannel = MidiReceiveChannelSplit::CH_2;
    msg.sendChannel = MidiSendChannel::CH_1;
    msg.sendSplitChannel = MidiSendChannelSplit::CH_2;

    msg.highVeloCCEnabled = false;
    msg.highResCCEnabled = false;

    msg.hwMappings = TestHelper::createFullMappings(false);

    for(auto hw : hws)
    {
      for(auto aspect : aspects)
      {
        TestHelper::updateMappingForHW(static_cast<int>(hw), msg.hwMappings, aspect, true);
      }
    }

    options.update(msg);
  };

  constexpr static uint8_t BASE_TCD = 0b11100000;
  constexpr static uint8_t Aftertouch = 4;
  constexpr static uint8_t Bender = 5;
  constexpr static auto sixteenThousand = 0b11111010000000;

  constexpr MidiEvent fullPressureTCDEventAftertouch
      = { BASE_TCD | Aftertouch, (uint8_t) (sixteenThousand >> 7), (uint8_t) (sixteenThousand & 127) };

  constexpr MidiEvent fullPressureTCDEventBender
      = { BASE_TCD | Bender, (uint8_t) (sixteenThousand >> 7), (uint8_t) (sixteenThousand & 127) };

  typedef nltools::msg::Setting::MidiSettingsMessage::MAPPING_INDEX tMAPPING_INDEX;
  typedef nltools::msg::Setting::MidiSettingsMessage::HW_INDEX tHW_INDEX;

  WHEN("Aftertouch")
  {
    bool didReceive = false;
    dsp.setType(SoundType::Split);

    dsp.setOnHWChangedCB(
        [&didReceive](auto hwID, auto, auto)
        {
          didReceive = true;
          CHECK(hwID == 4);
        });

    THEN("TCD gets send on Prim")
    {
      initSettings({ tHW_INDEX::Aftertouch }, { tMAPPING_INDEX::SEND_PRIMARY, tMAPPING_INDEX::LOCAL });

      eventStage.onTCDMessage(fullPressureTCDEventAftertouch);

      CHECK(didReceive);
      REQUIRE(sendMidiMessages.size() == 1);
    }

    THEN("TCD gets send on Split")
    {
      initSettings({ tHW_INDEX::Aftertouch }, { tMAPPING_INDEX::SEND_SPLIT, tMAPPING_INDEX::LOCAL });

      eventStage.onTCDMessage(fullPressureTCDEventAftertouch);

      CHECK(didReceive);
      REQUIRE(sendMidiMessages.size() == 1);
    }
  }

  WHEN("Bender")
  {
    bool didReceive = false;
    dsp.setType(SoundType::Split);

    dsp.setOnHWChangedCB(
        [&didReceive](auto hwID, auto, auto)
        {
            didReceive = true;
            CHECK(hwID == 5);
        });

    THEN("TCD gets send on Prim")
    {
      initSettings({ tHW_INDEX::Bender }, { tMAPPING_INDEX::SEND_PRIMARY, tMAPPING_INDEX::LOCAL });

      eventStage.onTCDMessage(fullPressureTCDEventBender);

      CHECK(didReceive);
      REQUIRE(sendMidiMessages.size() == 1);
    }

    THEN("TCD gets send on Split")
    {
      initSettings({ tHW_INDEX::Bender }, { tMAPPING_INDEX::SEND_SPLIT, tMAPPING_INDEX::LOCAL });

      eventStage.onTCDMessage(fullPressureTCDEventBender);

      CHECK(didReceive);
      REQUIRE(sendMidiMessages.size() == 1);
    }
  }
}
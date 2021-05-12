#include <catch.hpp>
#include <synth/input/InputEventStage.h>
#include <synth/C15Synth.h>
#include <mock/MockDSPHosts.h>
#include <mock/InputEventStageTester.h>

using InputEvent = DSPInterface::InputEventSource;

TEST_CASE("Interface correct", "[MIDI]")
{
  MockDSPHost host;
  MidiRuntimeOptions options;
  InputEventStage eventStage(&host, &options, [](auto) {});
  InputEventStageTester testObject(&eventStage);

  WHEN("Mappings: prim = none, sec = Common")
  {

    {
      nltools::msg::Setting::MidiSettingsMessage msg;
      msg.receiveNotes = true;
      msg.receiveChannel = MidiReceiveChannel::None;
      msg.receiveSplitChannel = MidiReceiveChannelSplit::Common;
      options.update(msg);
    }

    MIDIDecoder testDecoder(&host, &options);
    testDecoder.decode({ 0x90, 127, 127 });
    auto ret = testObject.getInputSourceFromParsedChannel(testDecoder.getChannel());
    CHECK(ret == InputEvent::Invalid);
  }

  WHEN("Mappings: prim = 1, sec = 1")
  {
    {
      nltools::msg::Setting::MidiSettingsMessage msg;
      msg.receiveNotes = true;
      msg.receiveChannel = MidiReceiveChannel::CH_1;
      msg.receiveSplitChannel = MidiReceiveChannelSplit::CH_1;
      options.update(msg);
    }

    MIDIDecoder testDecoder(&host, &options);
    testDecoder.decode({ 0x90, 127, 127 });
    auto ret = testObject.getInputSourceFromParsedChannel(testDecoder.getChannel());
    CHECK(ret == InputEvent::External_Both);
  }

  WHEN("Mappings: prim = 1, sec = 2")
  {
    {
      nltools::msg::Setting::MidiSettingsMessage msg;
      msg.receiveNotes = true;
      msg.receiveChannel = MidiReceiveChannel::CH_1;
      msg.receiveSplitChannel = MidiReceiveChannelSplit::CH_2;
      options.update(msg);
    }

    MIDIDecoder testDecoder(&host, &options);
    WHEN("Send on Channel 1")
    {
      testDecoder.decode({ 0x90, 127, 127 });
      auto ret = testObject.getInputSourceFromParsedChannel(testDecoder.getChannel());
      CHECK(ret == InputEvent::External_Primary);
    }

    WHEN("Send on Channel 2")
    {
      testDecoder.decode({ 0x91, 127, 127 });
      auto ret = testObject.getInputSourceFromParsedChannel(testDecoder.getChannel());
      CHECK(ret == InputEvent::External_Secondary);
    }
  }
}

namespace CCBitDetail
{
  void setSettings(MidiRuntimeOptions& options, MidiReceiveChannel r, MidiReceiveChannelSplit rs)
  {
    nltools::msg::Setting::MidiSettingsMessage msg;
    msg.receiveNotes = true;
    msg.receiveChannel = r;
    msg.receiveSplitChannel = rs;
    options.update(msg);
  }
}

TEST_CASE("MIDI Channel Mapping Tests", "[MIDI]")
{
  MockDSPHost host;
  MidiRuntimeOptions options;
  InputEventStage eventStage { &host, &options, [](auto) {} };
  InputEventStageTester t(&eventStage);

  // specific tests
  WHEN("Settings receive both none")
  {
    CCBitDetail::setSettings(options, MidiReceiveChannel::None, MidiReceiveChannelSplit::None);
    for(auto channel = 0; channel < 16; channel++)
    {
      auto testChannel = static_cast<MidiReceiveChannel>(channel + 2);
      auto ret = t.getInputSourceFromParsedChannel(testChannel);
      CHECK(ret == InputEvent::Invalid);
    }
  }

  WHEN("Settings receive none and common")
  {
    CCBitDetail::setSettings(options, MidiReceiveChannel::None, MidiReceiveChannelSplit::Common);
    for(auto channel = 0; channel < 16; channel++)
    {
      auto testChannel = static_cast<MidiReceiveChannel>(channel + 2);
      auto ret = t.getInputSourceFromParsedChannel(testChannel);
      CHECK(ret == InputEvent::Invalid);
    }
  }

  WHEN("Settings receive omni and common")
  {
    CCBitDetail::setSettings(options, MidiReceiveChannel::Omni, MidiReceiveChannelSplit::Common);
    for(auto channel = 0; channel < 16; channel++)
    {
      auto testChannel = static_cast<MidiReceiveChannel>(channel + 2);
      auto ret = t.getInputSourceFromParsedChannel(testChannel);
      CHECK(ret == InputEvent::External_Use_Split);
    }
  }

  WHEN("Settings receive 1 and common")
  {
    CCBitDetail::setSettings(options, MidiReceiveChannel::CH_1, MidiReceiveChannelSplit::Common);
    auto ret = t.getInputSourceFromParsedChannel(MidiReceiveChannel::CH_1);
    CHECK(ret == InputEvent::External_Use_Split);

    for(auto channel = 1; channel < 16; channel++)
    {
      auto testChannel = static_cast<MidiReceiveChannel>(channel + 2);
      auto ret2 = t.getInputSourceFromParsedChannel(testChannel);
      CHECK(ret2 == InputEvent::Invalid);
    }
  }

  WHEN("Settings receive Omni and 2")
  {
    CCBitDetail::setSettings(options, MidiReceiveChannel::Omni, MidiReceiveChannelSplit::CH_2);

    auto ret = t.getInputSourceFromParsedChannel(MidiReceiveChannel::CH_1);
    CHECK(ret == InputEvent::External_Primary);

    auto ret2 = t.getInputSourceFromParsedChannel(MidiReceiveChannel::CH_2);
    CHECK(ret2 == InputEvent::External_Both);

    for(auto channel = 2; channel < 16; channel++)
    {
      auto testChannel = static_cast<MidiReceiveChannel>(channel + 2);
      auto ret3 = t.getInputSourceFromParsedChannel(testChannel);
      CHECK(ret3 == InputEvent::External_Primary);
    }
  }

  WHEN("Settings receive 2 and 3")
  {
    CCBitDetail::setSettings(options, MidiReceiveChannel::CH_2, MidiReceiveChannelSplit::CH_3);

    auto ret = t.getInputSourceFromParsedChannel(MidiReceiveChannel::CH_1);
    CHECK(ret == InputEvent::Invalid);

    auto ret2 = t.getInputSourceFromParsedChannel(MidiReceiveChannel::CH_2);
    CHECK(ret2 == InputEvent::External_Primary);

    auto ret3 = t.getInputSourceFromParsedChannel(MidiReceiveChannel::CH_3);
    CHECK(ret3 == InputEvent::External_Secondary);

    for(auto channel = 3; channel < 16; channel++)
    {
      WHEN("Received Channel is " + std::to_string(channel))
      {
        auto testChannel = static_cast<MidiReceiveChannel>(channel + 2);
        auto ret4 = t.getInputSourceFromParsedChannel(testChannel);
        CHECK(ret4 == InputEvent::Invalid);
      }
    }
  }

  WHEN("Settings receive 3 and Omni")
  {
    CCBitDetail::setSettings(options, MidiReceiveChannel::CH_3, MidiReceiveChannelSplit::Omni);

    for(auto channel = 0; channel < 16; channel++)
    {
      WHEN("Received Channel is " + std::to_string(channel))
      {
        auto testChannel = static_cast<MidiReceiveChannel>(channel + 2);
        auto ret = t.getInputSourceFromParsedChannel(testChannel);

        if(testChannel == MidiReceiveChannel::CH_3)
        {
          CHECK(ret == InputEvent::External_Both);
        }
        else
        {
          CHECK(ret == InputEvent::External_Secondary);
        }
      }
    }
  }

  WHEN("Settings receive Omni and Omni")
  {
    CCBitDetail::setSettings(options, MidiReceiveChannel::Omni, MidiReceiveChannelSplit::Omni);

    for(auto channel = 0; channel < 16; channel++)
    {
      WHEN("Received Channel is " + std::to_string(channel))
      {
        auto testChannel = static_cast<MidiReceiveChannel>(channel + 2);
        auto ret = t.getInputSourceFromParsedChannel(testChannel);
        CHECK(ret == InputEvent::External_Both);
      }
    }
  }
}
#include <testing/TestHelper.h>
#include <synth/input/InputEventStage.h>
#include <MidiRuntimeOptions.h>
#include <synth/input/MidiChannelModeMessages.h>
#include <mock/MockDSPHosts.h>

TEST_CASE("Special Midi functions are recognises as such when received via midi")
{
  auto createCCMidiMessage = [](uint8_t channel, uint8_t cc, uint8_t value) {
      return MidiEvent{ static_cast<uint8_t>(0xB0 | channel), cc, value };
  };

  MidiRuntimeOptions options;

  {
    using tMSG = nltools::msg::Setting::MidiSettingsMessage;
    tMSG msg;
    msg.routings = TestHelper::createFullMappings(true);
    msg.receiveChannel = MidiReceiveChannel::Omni;
    TestHelper::updateMappingForHW(msg.routings,
                                   tMSG::RoutingIndex::Notes,
                                   tMSG::RoutingAspect::RECEIVE_PRIMARY,
                                   true);

    options.update(msg);
  }

  std::function<void(MidiChannelModeMessages)> currentCallback = nullptr;
  ConfigureableDSPHost host;
  InputEventStage foo { &host, &options, []() {}, [](auto) {},
                        [&currentCallback](auto f)
                        {
                          if(currentCallback)
                            currentCallback(f);
                        } };

  WHEN("Panic CC is Send with value 0")
  {
    currentCallback = [](auto f) {
      CHECK(f == MidiChannelModeMessages::AllSoundOff);
    };

    foo.onMIDIMessage(createCCMidiMessage(1, 120, 0));
  }

  WHEN("Panic CC is Send with value =! 0")
  {
    currentCallback = [](auto f) {
        CHECK(f == MidiChannelModeMessages::NOOP);
    };

    foo.onMIDIMessage(createCCMidiMessage(1, 120, 1));
  }
}
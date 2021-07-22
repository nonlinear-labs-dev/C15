#include <testing/TestHelper.h>
#include <MidiRuntimeOptions.h>
#include <synth/input/InputEventStage.h>
#include <mock/MockDSPHosts.h>

MidiRuntimeOptions createTCDSettings2()
{
  MidiRuntimeOptions options;
  nltools::msg::Setting::MidiSettingsMessage msg;

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

constexpr static uint8_t BASE_TCD = 0b11100000;
constexpr static uint8_t Ribbon1 = 0b00000110;

TEST_CASE("Ribbon 1 Return to Center")
{
  class MyTestHost : public PassOnHWReceived
  {
   public:
    using PassOnHWReceived::PassOnHWReceived;

    C15::Properties::HW_Return_Behavior getBehaviour(int id) override
    {
      return C15::Properties::HW_Return_Behavior::Center;
    }
  };

  MyTestHost dsp { Ribbon1, -1 };

  dsp.setType(SoundType::Single);
  auto settings = createTCDSettings2();
  InputEventStage eventStage { &dsp, &settings, [] {}, [](auto) {}, [](auto) {}};
  const auto zero = 0;

  CHECK_FALSE(dsp.didReceiveHW());

  WHEN("Ribbon 1 is Return to Center and TCD 0 was send")
  {
    eventStage.onTCDMessage({ BASE_TCD | Ribbon1, (uint8_t) (zero >> 7), (uint8_t) (zero & 127) });

    CHECK(dsp.didReceiveHW());
  }
}

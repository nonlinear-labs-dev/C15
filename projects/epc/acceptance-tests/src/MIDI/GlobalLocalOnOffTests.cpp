#include <testing/TestHelper.h>
#include <mock/MockDSPHosts.h>
#include <MidiRuntimeOptions.h>
#include <synth/input/InputEventStage.h>
#include <mock/InputEventStageTester.h>
#include <mock/MidiOptionsHelpers.h>
#include <mock/TCDHelpers.h>
#include <device-settings/midi/RoutingSettings.h>

TEST_CASE("'Global Local Enable' will be combined with 'RoutingSetting'")
{
  ConfigureableDSPHost dsp {};
  dsp.setType(SoundType::Split);

  MidiRuntimeOptions options {};
  InputEventStage eS(
      &dsp, &options, []() {}, [&](auto) {}, [](auto) {});

  WHEN("All Routings are On")
  {
    MidiOptionsHelper::configureOptions(&options,
                                        [](MidiOptionsHelper::tMSG& msg)
                                        {
                                          msg.routings = TestHelper::createFullMappings(true);
                                          msg.globalLocalEnable = true;
                                        });

    bool didReceive = false;
    dsp.setOnHWChangedCB([&didReceive](auto, auto, auto) { didReceive = true; });

    WHEN("HW Event is Send")
    {
      using namespace TCD_HELPER;
      eS.onTCDMessage(createFullPressureHWEvent(TCD_HW_IDS::Pedal1));
      CHECK(didReceive);
    }

    WHEN("HW Event is ignored when routing local is off")
    {
      MidiOptionsHelper::configureOptions(&options,
                                          [](MidiOptionsHelper::tMSG& msg)
                                          {
                                            msg.routings = TestHelper::createFullMappings(false);
                                            msg.globalLocalEnable = true;
                                          });

      using namespace TCD_HELPER;
      eS.onTCDMessage(createFullPressureHWEvent(TCD_HW_IDS::Pedal1));
      CHECK_FALSE(didReceive);
    }

    WHEN("HW Event is ignored when global local is off")
    {
      MidiOptionsHelper::configureOptions(&options,
                                          [](MidiOptionsHelper::tMSG& msg)
                                          {
                                            msg.routings = TestHelper::createFullMappings(true);
                                            msg.globalLocalEnable = false;
                                          });

      using namespace TCD_HELPER;
      eS.onTCDMessage(createFullPressureHWEvent(TCD_HW_IDS::Pedal1));
      CHECK_FALSE(didReceive);
    }

    WHEN("HW Event is ignored when global and routing local are off")
    {
      MidiOptionsHelper::configureOptions(&options,
                                          [](MidiOptionsHelper::tMSG& msg)
                                          {
                                            msg.routings = TestHelper::createFullMappings(false);
                                            msg.globalLocalEnable = false;
                                          });

      using namespace TCD_HELPER;
      eS.onTCDMessage(createFullPressureHWEvent(TCD_HW_IDS::Pedal1));
      CHECK_FALSE(didReceive);
    }
  }
}
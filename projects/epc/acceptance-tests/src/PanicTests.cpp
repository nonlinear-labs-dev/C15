#include <testing/TestHelper.h>
#include <mock/DspHostDualTester.h>
#include <AudioEngineOptions.h>
#include "Toolbox.h"
#include <synth/C15Synth.h>

TEST_CASE("Notes Active")
{
  using namespace std::chrono_literals;
  auto options = Tests::createEmptyAudioEngineOptions();
  auto synth = std::make_unique<C15Synth>(options.get());
  DspHostDualTester tester { synth->getDsp() };

  GIVEN("TCD KeyDown")
  {
    tester.applyTCDKeyDown(60, 1.0f, VoiceGroup::Global);
    synth->measurePerformance(20ms);
    const auto voices = tester.getActiveVoices(VoiceGroup::Global);
    THEN("One Active Voice")
    {
      CHECK(voices == 1);
    }
    WHEN("Same MIDI NoteOn")
    {
      tester.applyMidiNoteOn(61, 1.0f, MockInputEventSource::Primary, VoiceGroup::Global);
      synth->measurePerformance(20ms);

      THEN("Active Voices increased")
      {
        CHECK(tester.getActiveVoices(VoiceGroup::Global) > voices);

        WHEN("Panic Button Pressed")
        {
          synth->resetDSP();
          synth->measurePerformance(20ms);

          THEN("Active Voices Zero")
          {
            CHECK(tester.getActiveVoices(VoiceGroup::Global) == 0);
          }
        }
      }
    }
  }
}
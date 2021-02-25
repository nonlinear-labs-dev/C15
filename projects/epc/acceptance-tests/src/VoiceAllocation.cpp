#include <catch.hpp>
#include "Toolbox.h"
#include "AudioEngineOptions.h"
#include <synth/C15Synth.h>
#include <synth/c15-audio-engine/dsp_host_dual.h>

namespace Tests
{
  SCENARIO("Voices remain on preset change")
  {
    using namespace std::chrono_literals;

    auto options = createEmptyAudioEngineOptions();
    auto synth = std::make_unique<C15Synth>(options.get());
    auto preset1 = "dcc9b3d3-009d-4363-a64d-930c95d435a5";
    auto preset2 = "119284ae-b3d4-42a7-a155-31f04ed340ac";
    loadTestPreset(synth.get(), "voices-remain-on-preset-load", preset1);

    GIVEN("some notes are played without glitch suppression")
    {
      synth->getDsp()->onSettingGlitchSuppr(false);
      synth->doMidi({ 0x90, 0x50, 0x7F });
      synth->doMidi({ 0x90, 0x53, 0x7F });
      synth->doMidi({ 0x90, 0x56, 0x7F });

      synth->measurePerformance(250ms);

      WHEN("preset 2 is loaded")
      {
        loadTestPreset(synth.get(), "voices-remain-on-preset-load", preset2);
        synth->measurePerformance(250ms);

        THEN("sound is still playing")
        {
          auto result = synth->measurePerformance(250ms);
          REQUIRE(getMaxLevel(std::get<0>(result)) > 0.5f);
        }
      }
    }

    GIVEN("some notes are played with glitch suppression")
    {
      synth->getDsp()->onSettingGlitchSuppr(true);
      synth->doMidi({ 0x90, 0x50, 0x7F });
      synth->doMidi({ 0x90, 0x53, 0x7F });
      synth->doMidi({ 0x90, 0x56, 0x7F });

      synth->measurePerformance(250ms);

      WHEN("preset 2 is loaded")
      {
        loadTestPreset(synth.get(), "voices-remain-on-preset-load", preset2);
        synth->measurePerformance(250ms);

        THEN("sound is still playing")
        {
          auto result = synth->measurePerformance(250ms);
          REQUIRE(getMaxLevel(std::get<0>(result)) > 0.5f);
        }
      }
    }
  }
}

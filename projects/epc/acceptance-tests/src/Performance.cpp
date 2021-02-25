#include <catch.hpp>
#include "Toolbox.h"
#include "AudioEngineOptions.h"
#include <synth/C15Synth.h>

namespace Tests
{
  SCENARIO("CPU Usage is constant")
  {
    using namespace std::chrono_literals;

    auto options = createEmptyAudioEngineOptions();
    auto synth = std::make_unique<C15Synth>(options.get());
    loadTestPreset(synth.get(), "", "3c830098-c38b-461c-aa55-2548960c9064");

    auto withoutNote = synth->measurePerformance(1s);

    GIVEN("notes are played")
    {
      synth->doMidi({ 0x90, 0x50, 0x7F });

      auto withNote = synth->measurePerformance(4s);

      WHEN("audio decays")
      {
        synth->doMidi({ 0x80, 0x50, 0x7F });

        THEN("CPU usage is still the same after some sec")
        {
          synth->measurePerformance(30s);
          auto longAfterRelease = synth->measurePerformance(1s);
          auto diff = std::abs(std::get<1>(withoutNote) - std::get<1>(longAfterRelease));
          auto acceptedJitter = 0.1f;
          REQUIRE(diff > (1.0f - acceptedJitter));
          REQUIRE(diff < (1.0f + acceptedJitter));
        }
      }
    }
  }
}

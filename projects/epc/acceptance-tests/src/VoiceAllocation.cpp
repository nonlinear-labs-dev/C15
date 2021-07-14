#include <catch.hpp>
#include "Toolbox.h"
#include "AudioEngineOptions.h"
#include <synth/C15Synth.h>
#include <synth/c15-audio-engine/dsp_host_dual.h>
#include <mock/DspHostDualTester.h>

namespace Tests
{
  TEST_CASE("Initial State")
  {
    using namespace std::chrono_literals;
    auto options = createEmptyAudioEngineOptions();
    auto synth = std::make_unique<C15Synth>(options.get());
    DspHostDualTester tester { synth->getDsp() };
    THEN("No Active Voices")
    {
      CHECK(tester.getActiveVoices(VoiceGroup::Global) == 0);
    }
    THEN("Full Polyphony")
    {
      CHECK(tester.getAssignableVoices() == C15::Config::total_polyphony);
    }
    THEN("Silence")
    {
      const auto output = synth->measurePerformance(20ms);
      CHECK(tester.scanOutput(std::get<0>(output)).isSilence());
    }
  }
  TEST_CASE("Key Association")
  {
    using namespace std::chrono_literals;
    auto options = createEmptyAudioEngineOptions();
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
      WHEN("Same TCD KeyDown")
      {
        tester.applyTCDKeyDown(60, 1.0f, VoiceGroup::Global);
        synth->measurePerformance(20ms);
        THEN("Active Voices unchanged")
        {
          CHECK(tester.getActiveVoices(VoiceGroup::Global) == voices);
        }
      }
      WHEN("Same TCD KeyUp")
      {
        tester.applyTCDKeyUp(60, 1.0f, VoiceGroup::Global);
        synth->measurePerformance(20ms);
        THEN("No Active Voices")
        {
          CHECK(tester.getActiveVoices(VoiceGroup::Global) == 0);
        }
      }
      WHEN("Different TCD KeyDown")
      {
        tester.applyTCDKeyDown(61, 1.0f, VoiceGroup::Global);
        synth->measurePerformance(20ms);
        THEN("Active Voices increased")
        {
          CHECK(tester.getActiveVoices(VoiceGroup::Global) > voices);
        }
      }
      WHEN("Unpressed TCD KeyUp")
      {
        tester.applyTCDKeyUp(61, 1.0f, VoiceGroup::Global);
        synth->measurePerformance(20ms);
        THEN("Active Voices unchanged")
        {
          CHECK(tester.getActiveVoices(VoiceGroup::Global) == voices);
        }
      }
      WHEN("Same MIDI NoteOn")
      {
        tester.applyMidiNoteOn(60, 1.0f, MockInputEventSource::Primary, VoiceGroup::Global);
        synth->measurePerformance(20ms);
        THEN("Active Voices increased")
        {
          CHECK(tester.getActiveVoices(VoiceGroup::Global) > voices);
        }
        WHEN("Same MIDI NoteOff")
        {
          tester.applyMidiNoteOff(60, 1.0f, MockInputEventSource::Primary, VoiceGroup::Global);
          synth->measurePerformance(20ms);
          THEN("Active Voices decreased")
          {
            CHECK(tester.getActiveVoices(VoiceGroup::Global) == voices);
          }
        }
      }
    }
    GIVEN("MIDI NoteOn")
    {
      tester.applyMidiNoteOn(60, 1.0f, MockInputEventSource::Primary, VoiceGroup::Global);
      synth->measurePerformance(20ms);
      const auto voices = tester.getActiveVoices(VoiceGroup::Global);
      THEN("One Active Voice")
      {
        CHECK(voices == 1);
      }
      WHEN("Same MIDI NoteOn")
      {
        tester.applyMidiNoteOn(60, 1.0f, MockInputEventSource::Primary, VoiceGroup::Global);
        synth->measurePerformance(20ms);
        THEN("Active Voices unchanged")
        {
          CHECK(tester.getActiveVoices(VoiceGroup::Global) == voices);
        }
      }
      WHEN("Same MIDI NoteOff")
      {
        tester.applyMidiNoteOff(60, 1.0f, MockInputEventSource::Primary, VoiceGroup::Global);
        synth->measurePerformance(20ms);
        THEN("No Active Voices")
        {
          CHECK(tester.getActiveVoices(VoiceGroup::Global) == 0);
        }
      }
      WHEN("Different MIDI NoteOn")
      {
        tester.applyMidiNoteOn(61, 1.0f, MockInputEventSource::Primary, VoiceGroup::Global);
        synth->measurePerformance(20ms);
        THEN("Active Voices increased")
        {
          CHECK(tester.getActiveVoices(VoiceGroup::Global) > voices);
        }
      }
      WHEN("Unpressed MIDI NoteOff")
      {
        tester.applyMidiNoteOff(61, 1.0f, MockInputEventSource::Primary, VoiceGroup::Global);
        synth->measurePerformance(20ms);
        THEN("Active Voices unchanged")
        {
          CHECK(tester.getActiveVoices(VoiceGroup::Global) == voices);
        }
      }
      WHEN("Same TCD KeyDown")
      {
        tester.applyTCDKeyDown(60, 1.0f, VoiceGroup::Global);
        synth->measurePerformance(20ms);
        THEN("Active Voices increased")
        {
          CHECK(tester.getActiveVoices(VoiceGroup::Global) > voices);
        }
        WHEN("Same TCD KeyUp")
        {
          tester.applyTCDKeyUp(60, 1.0f, VoiceGroup::Global);
          synth->measurePerformance(20ms);
          THEN("Active Voices decreased")
          {
            CHECK(tester.getActiveVoices(VoiceGroup::Global) == voices);
          }
        }
      }
    }
    GIVEN("Distinguish Internal/Primary/Secondary Keys")
    {
      WHEN("Single Sound")
      {
        tester.applyMalformedSinglePreset({ 1, Polyphony::Poly });
        synth->measurePerformance(20ms);
        WHEN("TCD Key Down")
        {
          tester.applyTCDKeyDown(60, 1.0f, VoiceGroup::Global);
          synth->measurePerformance(20ms);
          THEN("One Active Voice")
          {
            CHECK(tester.getActiveVoices(VoiceGroup::Global) == 1);
          }
          WHEN("Same MIDI Primary NoteOn")
          {
            tester.applyMidiNoteOn(60, 1.0f, MockInputEventSource::Primary, VoiceGroup::Global);
            synth->measurePerformance(20ms);
            THEN("Two Active Voices")
            {
              CHECK(tester.getActiveVoices(VoiceGroup::Global) == 2);
            }
            WHEN("Same MIDI Secondary NoteOn")
            {
              tester.applyMidiNoteOn(60, 1.0f, MockInputEventSource::Secondary, VoiceGroup::Global);
              synth->measurePerformance(20ms);
              THEN("Active Voices unchanged")
              {
                CHECK(tester.getActiveVoices(VoiceGroup::Global) == 2);
              }
            }
          }
        }
      }
      WHEN("Split Sound")
      {
        tester.applyMalformedSplitPreset({ 1, Polyphony::Poly }, { 1, Polyphony::Poly });
        synth->measurePerformance(20ms);
        WHEN("TCD Key Down in Part I")
        {
          tester.applyTCDKeyDown(60, 1.0f, VoiceGroup::I);
          synth->measurePerformance(20ms);
          THEN("One Active Voice in Part I")
          {
            CHECK(tester.getActiveVoices(VoiceGroup::I) == 1);
          }
          WHEN("Same MIDI Primary NoteOn in Part I")
          {
            tester.applyMidiNoteOn(60, 1.0f, MockInputEventSource::Primary, VoiceGroup::I);
            synth->measurePerformance(20ms);
            THEN("Two Active Voices in Part I")
            {
              CHECK(tester.getActiveVoices(VoiceGroup::I) == 2);
            }
            WHEN("Same MIDI Secondary NoteOn in Part I")
            {
              tester.applyMidiNoteOn(60, 1.0f, MockInputEventSource::Secondary, VoiceGroup::I);
              synth->measurePerformance(20ms);
              THEN("Three Active Voices in Part I")
              {
                CHECK(tester.getActiveVoices(VoiceGroup::I) == 3);
              }
            }
          }
        }
      }
      WHEN("Layer Sound")
      {
        tester.applyMalformedLayerPreset({ 1, Polyphony::Poly });
        synth->measurePerformance(20ms);
        WHEN("TCD Key Down")
        {
          tester.applyTCDKeyDown(60, 1.0f, VoiceGroup::Global);
          synth->measurePerformance(20ms);
          THEN("One Active Voice in Part I")
          {
            CHECK(tester.getActiveVoices(VoiceGroup::I) == 1);
          }
          THEN("One Active Voice in Part II")
          {
            CHECK(tester.getActiveVoices(VoiceGroup::II) == 1);
          }
          WHEN("Same MIDI Primary NoteOn")
          {
            tester.applyMidiNoteOn(60, 1.0f, MockInputEventSource::Primary, VoiceGroup::Global);
            synth->measurePerformance(20ms);
            THEN("Two Active Voices in Part I")
            {
              CHECK(tester.getActiveVoices(VoiceGroup::I) == 2);
            }
            THEN("Two Active Voices in Part II")
            {
              CHECK(tester.getActiveVoices(VoiceGroup::II) == 2);
            }
            WHEN("Same MIDI Secondary NoteOn")
            {
              tester.applyMidiNoteOn(60, 1.0f, MockInputEventSource::Secondary, VoiceGroup::Global);
              synth->measurePerformance(20ms);
              THEN("Active Voices unchanged in Part I")
              {
                CHECK(tester.getActiveVoices(VoiceGroup::I) == 2);
              }
              THEN("Active Voices unchanged in Part II")
              {
                CHECK(tester.getActiveVoices(VoiceGroup::II) == 2);
              }
            }
          }
        }
      }
    }
  }
  TEST_CASE("128 Keys")
  {
    using namespace std::chrono_literals;
    auto options = createEmptyAudioEngineOptions();
    auto synth = std::make_unique<C15Synth>(options.get());
    DspHostDualTester tester { synth->getDsp() };
    GIVEN("Lowest TCD KeyDown")
    {
      tester.applyTCDKeyDown(0, 1.0f, VoiceGroup::Global);
      synth->measurePerformance(20ms);
      THEN("One Active Voice")
      {
        CHECK(tester.getActiveVoices(VoiceGroup::Global) == 1);
      }
      WHEN("Lowest TCD KeyUp")
      {
        tester.applyTCDKeyUp(0, 1.0f, VoiceGroup::Global);
        synth->measurePerformance(20ms);
        THEN("No Active Voices")
        {
          CHECK(tester.getActiveVoices(VoiceGroup::Global) == 0);
        }
      }
    }
    GIVEN("Highest TCD KeyDown")
    {
      tester.applyTCDKeyDown(127, 1.0f, VoiceGroup::Global);
      synth->measurePerformance(20ms);
      THEN("One Active Voice")
      {
        CHECK(tester.getActiveVoices(VoiceGroup::Global) == 1);
      }
      WHEN("Highest TCD KeyUp")
      {
        tester.applyTCDKeyUp(127, 1.0f, VoiceGroup::Global);
        synth->measurePerformance(20ms);
        THEN("No Active Voices")
        {
          CHECK(tester.getActiveVoices(VoiceGroup::Global) == 0);
        }
      }
    }
    GIVEN("Lowest MIDI NoteOn")
    {
      tester.applyMidiNoteOn(0, 1.0f, MockInputEventSource::Primary, VoiceGroup::Global);
      synth->measurePerformance(20ms);
      THEN("One Active Voice")
      {
        CHECK(tester.getActiveVoices(VoiceGroup::Global) == 1);
      }
      WHEN("Lowest MIDI NoteOff")
      {
        tester.applyMidiNoteOff(0, 1.0f, MockInputEventSource::Primary, VoiceGroup::Global);
        synth->measurePerformance(20ms);
        THEN("No Active Voices")
        {
          CHECK(tester.getActiveVoices(VoiceGroup::Global) == 0);
        }
      }
    }
    GIVEN("Highest MIDI NoteOn")
    {
      tester.applyMidiNoteOn(127, 1.0f, MockInputEventSource::Primary, VoiceGroup::Global);
      synth->measurePerformance(20ms);
      THEN("One Active Voice")
      {
        CHECK(tester.getActiveVoices(VoiceGroup::Global) == 1);
      }
      WHEN("Highest MIDI NoteOff")
      {
        tester.applyMidiNoteOff(127, 1.0f, MockInputEventSource::Primary, VoiceGroup::Global);
        synth->measurePerformance(20ms);
        THEN("No Active Voices")
        {
          CHECK(tester.getActiveVoices(VoiceGroup::Global) == 0);
        }
      }
    }
  }
  TEST_CASE("Reset Behavior")
  {
    using namespace std::chrono_literals;
    auto options = createEmptyAudioEngineOptions();
    auto synth = std::make_unique<C15Synth>(options.get());
    DspHostDualTester tester { synth->getDsp() };
    GIVEN("Active Voices in Single Sound")
    {
      tester.applyMalformedSinglePreset({ 1, Polyphony::Poly });
      synth->measurePerformance(20ms);
      tester.applyTCDKeyDown(60, 1.0f, VoiceGroup::Global);
      synth->measurePerformance(20ms);
      THEN("One Active Voice")
      {
        CHECK(tester.getActiveVoices(VoiceGroup::Global) == 1);
      }
      WHEN("Generic Reset Events")
      {
        WHEN("Reset Synth")
        {
          synth->resetDSP();
          synth->measurePerformance(20ms);
          THEN("No Active Voices")
          {
            CHECK(tester.getActiveVoices(VoiceGroup::Global) == 0);
          }
        }
        WHEN("MIDI Settings Message")
        {
          synth->onMidiSettingsMessage(nltools::msg::Setting::MidiSettingsMessage {});
          synth->measurePerformance(20ms);
          THEN("No Active Voices")
          {
            CHECK(tester.getActiveVoices(VoiceGroup::Global) == 0);
          }
        }
      }
      WHEN("Parameter Changed Messages")
      {
        WHEN("Change Mono Enable Parameter")
        {
          tester.applyMonoMessage(Polyphony::Mono, VoiceGroup::Global);
          synth->measurePerformance(20ms);
          THEN("No Active Voices")
          {
            CHECK(tester.getActiveVoices(VoiceGroup::Global) == 0);
          }
        }
        WHEN("Change Unison Voices Parameter")
        {
          tester.applyUnisonMessage(2, VoiceGroup::Global);
          synth->measurePerformance(20ms);
          THEN("No Active Voices")
          {
            CHECK(tester.getActiveVoices(VoiceGroup::Global) == 0);
          }
        }
      }
      WHEN("Preset Messages")
      {
        WHEN("Same Sound Type")
        {
          WHEN("Single Preset with identical Mono/Unison")
          {
            tester.applyMalformedSinglePreset({ 1, Polyphony::Poly });
            synth->measurePerformance(20ms);
            THEN("One Active Voice")
            {
              CHECK(tester.getActiveVoices(VoiceGroup::Global) == 1);
            }
          }
          WHEN("Single Preset with different Mono Enable")
          {
            tester.applyMalformedSinglePreset({ 1, Polyphony::Mono });
            synth->measurePerformance(20ms);
            THEN("No Active Voices")
            {
              CHECK(tester.getActiveVoices(VoiceGroup::Global) == 0);
            }
          }
          WHEN("Single Preset with different Unison Voices")
          {
            tester.applyMalformedSinglePreset({ 2, Polyphony::Poly });
            synth->measurePerformance(20ms);
            THEN("No Active Voices")
            {
              CHECK(tester.getActiveVoices(VoiceGroup::Global) == 0);
            }
          }
        }
        WHEN("Different Sound Type")
        {
          WHEN("Split Preset with identical Mono/Unison in both Parts")
          {
            tester.applyMalformedSplitPreset({ 1, Polyphony::Poly }, { 1, Polyphony::Poly });
            synth->measurePerformance(20ms);
            THEN("No Active Voices")
            {
              CHECK(tester.getActiveVoices(VoiceGroup::Global) == 0);
            }
          }
          WHEN("Layer Preset with identical Mono/Unison")
          {
            tester.applyMalformedLayerPreset({ 1, Polyphony::Poly });
            synth->measurePerformance(20ms);
            THEN("No Active Voices")
            {
              CHECK(tester.getActiveVoices(VoiceGroup::Global) == 0);
            }
          }
        }
      }
    }
    GIVEN("Active Voices in Split Sound")
    {
      tester.applyMalformedSplitPreset({ 1, Polyphony::Poly }, { 1, Polyphony::Poly });
      synth->measurePerformance(20ms);
      tester.applyTCDKeyDown(60, 1.0f, VoiceGroup::Global);
      synth->measurePerformance(20ms);
      THEN("One Active Voice in Part I")
      {
        CHECK(tester.getActiveVoices(VoiceGroup::I) == 1);
      }
      THEN("One Active Voice in Part II")
      {
        CHECK(tester.getActiveVoices(VoiceGroup::II) == 1);
      }
      WHEN("Generic Reset Events")
      {
        WHEN("Reset Synth")
        {
          synth->resetDSP();
          synth->measurePerformance(20ms);
          THEN("No Active Voices")
          {
            CHECK(tester.getActiveVoices(VoiceGroup::Global) == 0);
          }
        }
        WHEN("MIDI Settings Message")
        {
          synth->onMidiSettingsMessage(nltools::msg::Setting::MidiSettingsMessage {});
          synth->measurePerformance(20ms);
          THEN("No Active Voices")
          {
            CHECK(tester.getActiveVoices(VoiceGroup::Global) == 0);
          }
        }
      }
      WHEN("Parameter Changed Messages")
      {
        WHEN("In Part I")
        {
          WHEN("Change Mono Enable Parameter")
          {
            tester.applyMonoMessage(Polyphony::Mono, VoiceGroup::I);
            synth->measurePerformance(20ms);
            THEN("No Active Voices in Part I")
            {
              CHECK(tester.getActiveVoices(VoiceGroup::I) == 0);
            }
            THEN("One Active Voice in Part II")
            {
              CHECK(tester.getActiveVoices(VoiceGroup::II) == 1);
            }
          }
          WHEN("Change Unison Voices Parameter")
          {
            tester.applyUnisonMessage(2, VoiceGroup::I);
            synth->measurePerformance(20ms);
            THEN("No Active Voices in Part I")
            {
              CHECK(tester.getActiveVoices(VoiceGroup::I) == 0);
            }
            THEN("One Active Voice in Part II")
            {
              CHECK(tester.getActiveVoices(VoiceGroup::II) == 1);
            }
          }
        }
        WHEN("In Part II")
        {
          WHEN("Change Mono Enable Parameter")
          {
            tester.applyMonoMessage(Polyphony::Mono, VoiceGroup::II);
            synth->measurePerformance(20ms);
            THEN("One Active Voice in Part I")
            {
              CHECK(tester.getActiveVoices(VoiceGroup::I) == 1);
            }
            THEN("No Active Voices in Part II")
            {
              CHECK(tester.getActiveVoices(VoiceGroup::II) == 0);
            }
          }
          WHEN("Change Unison Voices Parameter")
          {
            tester.applyUnisonMessage(2, VoiceGroup::II);
            synth->measurePerformance(20ms);
            THEN("One Active Voice in Part I")
            {
              CHECK(tester.getActiveVoices(VoiceGroup::I) == 1);
            }
            THEN("No Active Voices in Part II")
            {
              CHECK(tester.getActiveVoices(VoiceGroup::II) == 0);
            }
          }
        }
      }
      WHEN("Preset Messages")
      {
        WHEN("Same Sound Type")
        {
          WHEN("Split Preset with identical Mono/Unison in both Parts")
          {
            tester.applyMalformedSplitPreset({ 1, Polyphony::Poly }, { 1, Polyphony::Poly });
            synth->measurePerformance(20ms);
            THEN("One Active Voice in Part I")
            {
              CHECK(tester.getActiveVoices(VoiceGroup::I) == 1);
            }
            THEN("One Active Voice in Part II")
            {
              CHECK(tester.getActiveVoices(VoiceGroup::II) == 1);
            }
          }
          WHEN("Split Preset with different Mono Enable")
          {
            WHEN("In Part I")
            {
              tester.applyMalformedSplitPreset({ 1, Polyphony::Mono }, { 1, Polyphony::Poly });
              synth->measurePerformance(20ms);
              THEN("No Active Voices in Part I")
              {
                CHECK(tester.getActiveVoices(VoiceGroup::I) == 0);
              }
              THEN("One Active Voice in Part II")
              {
                CHECK(tester.getActiveVoices(VoiceGroup::II) == 1);
              }
            }
            WHEN("In Part II")
            {
              tester.applyMalformedSplitPreset({ 1, Polyphony::Poly }, { 1, Polyphony::Mono });
              synth->measurePerformance(20ms);
              THEN("One Active Voice in Part I")
              {
                CHECK(tester.getActiveVoices(VoiceGroup::I) == 1);
              }
              THEN("No Active Voices in Part II")
              {
                CHECK(tester.getActiveVoices(VoiceGroup::II) == 0);
              }
            }
          }
          WHEN("Split Preset with different Unison Voices")
          {
            WHEN("In Part I")
            {
              tester.applyMalformedSplitPreset({ 2, Polyphony::Poly }, { 1, Polyphony::Poly });
              synth->measurePerformance(20ms);
              THEN("No Active Voices in Part I")
              {
                CHECK(tester.getActiveVoices(VoiceGroup::I) == 0);
              }
              THEN("One Active Voice in Part II")
              {
                CHECK(tester.getActiveVoices(VoiceGroup::II) == 1);
              }
            }
            WHEN("In Part II")
            {
              tester.applyMalformedSplitPreset({ 1, Polyphony::Poly }, { 2, Polyphony::Poly });
              synth->measurePerformance(20ms);
              THEN("One Active Voice in Part I")
              {
                CHECK(tester.getActiveVoices(VoiceGroup::I) == 1);
              }
              THEN("No Active Voices in Part II")
              {
                CHECK(tester.getActiveVoices(VoiceGroup::II) == 0);
              }
            }
          }
        }
        WHEN("Different Sound Type")
        {
          WHEN("Single Preset with identical Mono/Unison")
          {
            tester.applyMalformedSinglePreset({ 1, Polyphony::Poly });
            synth->measurePerformance(20ms);
            THEN("No Active Voices")
            {
              CHECK(tester.getActiveVoices(VoiceGroup::Global) == 0);
            }
          }
          WHEN("Layer Preset with identical Mono/Unison")
          {
            tester.applyMalformedLayerPreset({ 1, Polyphony::Poly });
            synth->measurePerformance(20ms);
            THEN("No Active Voices")
            {
              CHECK(tester.getActiveVoices(VoiceGroup::Global) == 0);
            }
          }
        }
      }
    }
    GIVEN("Active Voices in Layer Sound")
    {
      tester.applyMalformedLayerPreset({ 1, Polyphony::Poly });
      synth->measurePerformance(20ms);
      tester.applyTCDKeyDown(60, 1.0f, VoiceGroup::Global);
      synth->measurePerformance(20ms);
      THEN("One Active Voice in Part I")
      {
        CHECK(tester.getActiveVoices(VoiceGroup::I) == 1);
      }
      THEN("One Active Voice in Part II")
      {
        CHECK(tester.getActiveVoices(VoiceGroup::II) == 1);
      }
      WHEN("Generic Reset Events")
      {
        WHEN("Reset Synth")
        {
          synth->resetDSP();
          synth->measurePerformance(20ms);
          THEN("No Active Voices")
          {
            CHECK(tester.getActiveVoices(VoiceGroup::Global) == 0);
          }
        }
        WHEN("MIDI Settings Message")
        {
          synth->onMidiSettingsMessage(nltools::msg::Setting::MidiSettingsMessage {});
          synth->measurePerformance(20ms);
          THEN("No Active Voices")
          {
            CHECK(tester.getActiveVoices(VoiceGroup::Global) == 0);
          }
        }
      }
      WHEN("Parameter Messages")
      {
        WHEN("Change Mono Enable Parameter")
        {
          tester.applyMonoMessage(Polyphony::Mono, VoiceGroup::Global);
          synth->measurePerformance(20ms);
          THEN("No Active Voices")
          {
            CHECK(tester.getActiveVoices(VoiceGroup::Global) == 0);
          }
        }
        WHEN("Change Unison Voices Parameter")
        {
          tester.applyUnisonMessage(2, VoiceGroup::Global);
          synth->measurePerformance(20ms);
          THEN("No Active Voices")
          {
            CHECK(tester.getActiveVoices(VoiceGroup::Global) == 0);
          }
        }
      }
      WHEN("Preset Messages")
      {
        WHEN("Same Sound Type")
        {
          WHEN("Layer Preset with identical Mono/Unison")
          {
            tester.applyMalformedLayerPreset({ 1, Polyphony::Poly });
            synth->measurePerformance(20ms);
            THEN("One Active Voice in Part I")
            {
              CHECK(tester.getActiveVoices(VoiceGroup::I) == 1);
            }
            THEN("One Active Voice in Part II")
            {
              CHECK(tester.getActiveVoices(VoiceGroup::II) == 1);
            }
          }
          WHEN("Layer Preset with different Mono Enable")
          {
            tester.applyMalformedLayerPreset({ 1, Polyphony::Mono });
            synth->measurePerformance(20ms);
            THEN("No Active Voices")
            {
              CHECK(tester.getActiveVoices(VoiceGroup::Global) == 0);
            }
          }
          WHEN("Layer Preset with different Unison Voices")
          {
            tester.applyMalformedLayerPreset({ 2, Polyphony::Poly });
            synth->measurePerformance(20ms);
            THEN("No Active Voices")
            {
              CHECK(tester.getActiveVoices(VoiceGroup::Global) == 0);
            }
          }
        }
        WHEN("Different Sound Type")
        {
          WHEN("Single Preset with identical Mono/Unison")
          {
            tester.applyMalformedSinglePreset({ 1, Polyphony::Poly });
            synth->measurePerformance(20ms);
            THEN("No Active Voices")
            {
              CHECK(tester.getActiveVoices(VoiceGroup::Global) == 0);
            }
          }
          WHEN("Split Preset with identical Mono/Unison in both Parts")
          {
            tester.applyMalformedSplitPreset({ 1, Polyphony::Poly }, { 1, Polyphony::Poly });
            synth->measurePerformance(20ms);
            THEN("No Active Voices")
            {
              CHECK(tester.getActiveVoices(VoiceGroup::Global) == 0);
            }
          }
        }
      }
    }
  }
}

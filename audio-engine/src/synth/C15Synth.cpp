#include "C15Synth.h"
#include "c15-audio-engine/dsp_host.h"
#include "main.h"
#include "Options.h"
#include "io/Log.h"

C15Synth::C15Synth()
    : m_dsp(std::make_unique<dsp_host>())
{
  m_dsp->init(getOptions()->getSampleRate(), getOptions()->getPolyphony());
}

C15Synth::~C15Synth() = default;

void C15Synth::doMidi(const MidiEvent &event)
{
#if test_inputModeFlag
  m_dsp->testMidi(event.raw[0], event.raw[1], event.raw[2]);
#else
  m_dsp->evalMidi(event.raw[0], event.raw[1], event.raw[2]);
#endif
}

void C15Synth::doAudio(SampleFrame *target, size_t numFrames)
{
  for(size_t i = 0; i < numFrames; i++)
  {
    m_dsp->tickMain();
    target[i].left = m_dsp->m_mainOut_L;
    target[i].right = m_dsp->m_mainOut_R;
  }
}

double C15Synth::measurePerformance(std::chrono::seconds time)
{
  m_dsp->testLoadPreset(4);
  Synth::measurePerformance(time);
}

void C15Synth::printAndResetTcdInputLog()
{
  auto cp = m_dsp->m_tcd_input_log;
  m_dsp->m_tcd_input_log.reset();

  Log::info("TCD MIDI Input Log: (Length:", cp.m_length, ")");

  if(cp.m_length > 0)
  {
    Log::info("(elapsed Samples\t: Command Argument, ...) - Argument is always unsigned 14 bit");

    for(uint32_t i = 0; i < cp.m_length; i++)
    {
      const auto &e = cp.m_entry[(cp.m_startPos + i) % tcd_log_buffersize];

      if(e.m_time > 0)
        Log::info(e.m_time, "\t: ", tcd_command_names[e.m_cmdId], e.m_arg, ", ");
      else
        Log::info(tcd_command_names[e.m_cmdId], e.m_arg, ", ");
    }

    Log::info("End of TCD MIDI Input Log");
  }
}

void C15Synth::resetDSP()
{
  m_dsp->resetDSP();
  Log::info("DSP has been reset.");
}

void C15Synth::toggleTestTone()
{
  auto tmp = static_cast<float>(1 - m_dsp->m_test_tone.m_state);
  m_dsp->m_decoder.m_utilityId = 4;
  m_dsp->utilityUpdate(tmp);
  Log::info("Test Tone toggled:", m_dsp->m_test_tone.m_state);
}

void C15Synth::selectTestToneFrequency()
{
  m_dsp->m_test_tone.m_focus = 0;
  Log::info("Test Tone: Frequency:\t", m_dsp->m_test_tone.a_frequency, "Hz");
}

void C15Synth::selectTestToneAmplitude()
{
  m_dsp->m_test_tone.m_focus = 1;
  Log::info("Test Tone: Amplitude:\t", m_dsp->m_test_tone.a_amplitude, "dB");
}

void C15Synth::increase()
{
  changeSelectedValueBy(1);
}

void C15Synth::decrease()
{
  changeSelectedValueBy(-1);
}

void C15Synth::changeSelectedValueBy(int i)
{
  switch(m_dsp->m_test_tone.m_focus)
  {
    case 0:
    {
      m_dsp->m_decoder.m_utilityId = 2;
      auto tmp = std::clamp(m_dsp->m_test_tone.a_frequency + i * 10.f, 0.f, 1000.f);
      m_dsp->utilityUpdate(tmp);
      Log::info("Test Tone: Frequency:\t", tmp, "Hz");
    }
    break;
    case 1:
    {
      m_dsp->m_decoder.m_utilityId = 3;
      auto tmp = std::clamp(m_dsp->m_test_tone.a_amplitude + i * 1.f, -60.f, 0.f);
      m_dsp->utilityUpdate(tmp);
      Log::info("Test Tone: Amplitude:\t", tmp, "dB");
    }
    break;
  }
}

#include "CPUBurningSynth.h"
#include "AudioEngineOptions.h"
#include <math.h>

CPUBurningSynth::CPUBurningSynth(const AudioEngineOptions *options)
    : Synth(options)
{
}

void CPUBurningSynth::doMidi(const MidiEvent &)
{
}

void CPUBurningSynth::doAudio(SampleFrame *target, size_t numFrames)
{
  for(size_t i = 0; i < numFrames; i++)
  {
    target[i].left = target[i].right = 0;

    for(int j = 0; j < getOptions()->getNumCpuBurningSines(); j++)
    {
      target[i].left += 0.000001f * std::sin(m_phase++ / 1000);
      target[i].right += 0.000001f * std::sin(m_phase++ / 1000);

      if(m_phase > 1000)
        m_phase = 0;
    }

    target[i].left *= 0.000001f;
    target[i].right *= 0.000001f;
  }
}

#include "SimpleSynth.h"
#include "AudioEngineOptions.h"
#include <iostream>
#include <math.h>

SimpleSynth::SimpleSynth(const AudioEngineOptions *options)
    : Synth(options)
{
}

void SimpleSynth::doMidi(const MidiEvent &event)
{
  auto b = event.raw[0] & 0xF0;

  if(b == 0x90)
  {
    auto note = event.raw[1];

    m_voices[note].phase = 0;
    m_voices[note].vol = 1.0;

    auto hertz = 440.f * powf(2.f, (note - 69) / 12.0f);
    m_voices[note].inc = hertz / getOptions()->getSampleRate();
  }
}

void SimpleSynth::doAudio(SampleFrame *target, size_t numFrames)
{
  const float volumeDegradation = 1 / powf(20, 1.f / getOptions()->getSampleRate());

  for(size_t i = 0; i < numFrames; i++)
  {
    target[i].left = 0;
    target[i].right = 0;

    for(auto &v : m_voices)
    {
      float sample = sinf(v.phase * 2.f * static_cast<float>(M_PI)) * v.vol;
      v.phase += v.inc;

      if(v.phase > 1)
        v.phase -= 1;

      v.vol = v.vol * volumeDegradation + std::numeric_limits<float>::min();

      target[i].left += 0.25f * sample;
      target[i].right += 0.25f * sample;
    }
  }
}

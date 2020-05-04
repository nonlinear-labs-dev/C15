#pragma once

#include "Synth.h"

class CPUBurningSynth : public Synth
{
 public:
  CPUBurningSynth(const AudioEngineOptions *options);

  void doMidi(const MidiEvent &event) override;
  void doAudio(SampleFrame *target, size_t numFrames) override;

 private:
  float m_phase = 0;
};

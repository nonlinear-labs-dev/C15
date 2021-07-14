#pragma once

#include <memory>
#include <synth/Synth.h>

class AudioEngineOptions;
class C15Synth;

namespace Tests
{
  std::unique_ptr<AudioEngineOptions> createEmptyAudioEngineOptions();
  void dumpAudioData(const Synth::AudioBlock &audio, const std::string &file);
  float getMaxLevel(const Synth::AudioBlock &audio);
}

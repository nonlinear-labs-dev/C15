#pragma once

#include <memory>
#include <synth/Synth.h>

class AudioEngineOptions;
class C15Synth;

namespace Tests
{
  std::unique_ptr<AudioEngineOptions> createEmptyAudioEngineOptions();
  void loadTestPreset(C15Synth *synth, const std::string &subDir, const std::string &uuid);
  void dumpAudioData(const Synth::AudioBlock &audio, const std::string &file);
  float getMaxLevel(const Synth::AudioBlock &audio);
}

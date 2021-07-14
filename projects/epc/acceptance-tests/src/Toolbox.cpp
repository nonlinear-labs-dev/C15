#include "Toolbox.h"
#include <AudioEngineOptions.h>
#include <http/UpdateDocumentMaster.h>
#include <libundo/undo/Scope.h>
#include <CompileTimeOptions.h>
#include <giomm.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <presets/Preset.h>
#include <synth/C15Synth.h>
#include <proxies/audio-engine/AudioEngineProxy.h>
#include <fstream>

namespace Tests
{
  std::unique_ptr<AudioEngineOptions> createEmptyAudioEngineOptions()
  {
    int argc = 0;
    char **argv = nullptr;
    return std::make_unique<AudioEngineOptions>(argc, argv);
  }

  void dumpAudioData(const Synth::AudioBlock &audio, const std::string &fileName)
  {
    std::ofstream file(fileName);
    file.write(reinterpret_cast<const char *>(audio.data()),
               static_cast<std::streamsize>(audio.size() * sizeof(SampleFrame)));
  }

  float getMaxLevel(const Synth::AudioBlock &audio)
  {
    float v = 0;
    for(const auto &a : audio)
    {
      v = std::max(std::abs(a.left), v);
      v = std::max(std::abs(a.right), v);
    }
    return v;
  }

}

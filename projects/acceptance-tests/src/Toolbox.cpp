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
  class UpdateDocumentMasterMock : public UpdateDocumentMaster
  {
   public:
    void writeDocument(Writer &, tUpdateID) const override
    {
    }
  };

  std::unique_ptr<AudioEngineOptions> createEmptyAudioEngineOptions()
  {
    int argc = 0;
    char **argv = nullptr;
    return std::make_unique<AudioEngineOptions>(argc, argv);
  }

  void loadTestPreset(C15Synth *synth, const std::string &subDir, const std::string &uuid)
  {
    UpdateDocumentMasterMock updateDocMaster;
    UNDO::Scope undoScope(&updateDocMaster);

    auto presetData = getSourceDir() + "/projects/acceptance-tests/test-data";

    if(!subDir.empty())
      presetData += "/" + subDir;

    auto file = Gio::File::create_for_path(presetData);
    auto transactionScope = undoScope.startTransaction("load");
    auto transaction = transactionScope->getTransaction();

    PresetManager pm(&updateDocMaster, true);
    EditBuffer editBuffer(&pm);
    Preset preset(&pm);
    preset.setUuid(transaction, uuid);
    preset.load(transaction, file);
    editBuffer.copyFrom(transaction, &preset);

    switch(editBuffer.getType())
    {
      case SoundType::Single:
        synth->onSinglePresetMessage(AudioEngineProxy::createSingleEditBufferMessage(editBuffer));
        break;

      case SoundType::Split:
        synth->onSplitPresetMessage(AudioEngineProxy::createSplitEditBufferMessage(editBuffer));
        break;

      case SoundType::Layer:
        synth->onLayerPresetMessage(AudioEngineProxy::createLayerEditBufferMessage(editBuffer));
        break;

      default:
        break;
    }
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

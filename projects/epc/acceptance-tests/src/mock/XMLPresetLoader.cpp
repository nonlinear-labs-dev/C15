#include "XMLPresetLoader.h"
#include <AudioEngineOptions.h>
#include <mock/UpdateDocumentMasterMock.h>
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
#include <use-cases/PresetManagerUseCases.h>
#include <presets/Bank.h>

void XMLPresetLoader::loadTestPreset(C15Synth *synth, const std::string &subDir, const std::string &uuid)
{
  UpdateDocumentMasterMock updateDocMaster;
  UNDO::Scope undoScope(&updateDocMaster);

  auto presetData = getSourceDir() + "/projects/epc/acceptance-tests/test-data";

  if(!subDir.empty())
    presetData += "/" + subDir;

  auto file = Gio::File::create_for_path(presetData);
  auto transactionScope = undoScope.startTransaction("load");
  auto transaction = transactionScope->getTransaction();

  PresetManager pm(&updateDocMaster, true);
  EditBuffer editBuffer(&pm);
  Preset preset(&pm);
  preset.setUuid(transaction, Uuid { uuid });
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

void XMLPresetLoader::loadTestPresetFromBank(C15Synth *synth, const std::string &subDir,
                                             const std::string &bankFileName)
{
  UpdateDocumentMasterMock updateDocMaster;
  UNDO::Scope undoScope(&updateDocMaster);

  auto presetData = getSourceDir() + "/projects/epc/acceptance-tests/test-data";

  if(!subDir.empty())
    presetData += "/" + subDir;

  presetData += "/" + bankFileName + ".xml";

  auto file = Gio::File::create_for_path(presetData);
  auto transactionScope = undoScope.startTransaction("load");
  auto transaction = transactionScope->getTransaction();

  PresetManager pm(&updateDocMaster, true);
  PresetManagerUseCases useCase(&pm);

  useCase.importBankFromPath(std::filesystem::directory_entry { presetData });

  auto bank = pm.getBankAt(0);
  auto preset = bank->getPresetAt(0);

  nltools_assertAlways(bank != nullptr);
  nltools_assertAlways(preset != nullptr);

  EditBuffer editBuffer(&pm);
  editBuffer.copyFrom(transaction, preset);

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

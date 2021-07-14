#include "EditBufferActions.h"
#include "EditBuffer.h"
#include "parameters/MacroControlParameter.h"
#include <parameters/RibbonParameter.h>
#include <parameters/PedalParameter.h>
#include <parameters/ModulateableParameter.h>
#include <http/HTTPRequest.h>
#include <Application.h>
#include <device-settings/Settings.h>
#include <proxies/playcontroller/PlaycontrollerProxy.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/audio-engine/AudioEngineProxy.h>
#include <groups/ParameterGroup.h>
#include <boost/algorithm/string.hpp>
#include <presets/PresetManager.h>
#include <presets/Preset.h>
#include <http/UndoScope.h>
#include <parameters/names/ParameterDB.h>
#include <parameter_declarations.h>
#include <http/SoupOutStream.h>
#include <parameters/SplitPointParameter.h>
#include <use-cases/ParameterUseCases.h>
#include <use-cases/ModParameterUseCases.h>
#include <use-cases/RibbonParameterUseCases.h>
#include <use-cases/PedalParameterUseCases.h>
#include <iomanip>
#include <presets/LoadedPresetLog.h>

//NonMember helperFunctions pre:
IntrusiveList<EditBufferActions::tParameterPtr> getScaleParameters(EditBuffer* editBuffer);

EditBufferActions::EditBufferActions(EditBuffer* editBuffer)
    : super("/presets/param-editor/")
{
  addAction("sync-audioengine", [=](std::shared_ptr<NetworkRequest> request) mutable {
    Application::get().getAudioEngineProxy()->sendEditBuffer();
  });

  addAction("restore", [=](std::shared_ptr<NetworkRequest> request) mutable {
    using namespace std::chrono;
    auto time = std::stoll(request->get("timestamp"));
    auto& scope = editBuffer->getUndoScope();

    if(auto transaction = scope.findTransactionAt(system_clock::time_point(system_clock::duration(time))))
    {
      auto currentTip = scope.getUndoTransaction();
      auto log = editBuffer->getLoadedPresetLog();
      log->freeze();
      scope.undoJump(UNDO::StringTools::buildString(reinterpret_cast<size_t>(transaction)));
      auto p = std::make_unique<Preset>(editBuffer->getParent(), *editBuffer, Uuid::restored());
      scope.undoJump(UNDO::StringTools::buildString(reinterpret_cast<size_t>(currentTip)));
      EditBufferUseCases ebUseCases(editBuffer);
      std::stringstream nameBuilder;
      std::time_t tt = system_clock::to_time_t(system_clock::time_point(system_clock::duration(time)));
      std::tm tm = *std::localtime(&tt);
      nameBuilder << "Restore at " << std::put_time(&tm, "%H:%M:%S");
      ebUseCases.undoableLoad(p.get(), nameBuilder.str());
      log->thaw();
    }
  });

  addAction("select-param", [=](std::shared_ptr<NetworkRequest> request) mutable {
    Glib::ustring id = request->get("id");
    EditBufferUseCases ebUseCases(editBuffer);
    ebUseCases.selectParameter(ParameterId(id), false);
  });

  addAction("set-param", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto id = request->get("id");
    auto value = std::stod(request->get("value"));
    EditBufferUseCases ebUseCases(editBuffer);
    ebUseCases.setParameter(ParameterId(id), value);
  });

  addAction("set-splits", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto id = request->get("id");
    auto value = std::stod(request->get("value"));
    auto otherValue = std::stod(request->get("other-value"));
    EditBufferUseCases ebUseCases(editBuffer);
    ebUseCases.setSplits(ParameterId(id), value, otherValue);
  });

  addAction("set-mod-amount", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto amount = std::stod(request->get("amount"));
    auto id = request->get("id");
    EditBufferUseCases ebUseCases(editBuffer);
    ebUseCases.setModulationAmount(amount, ParameterId { id });
  });

  addAction("set-mod-src", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto src = std::stoi(request->get("source"));
    auto id = request->get("id");
    EditBufferUseCases ebUseCases(editBuffer);
    ebUseCases.setModulationSource(static_cast<MacroControls>(src), ParameterId { id });
  });

  addAction("reset", [=](std::shared_ptr<NetworkRequest>) mutable {
    SoundUseCases useCase { editBuffer, editBuffer->getParent() };
    useCase.initSound();
  });

  addAction("rename-mc", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto id = request->get("id");
    auto newName = request->get("new-name");
    EditBufferUseCases useCase { editBuffer };
    if(auto mcUseCase = useCase.getMCUseCase(ParameterId { id }))
    {
      mcUseCase->setName(newName);
    }
  });

  addAction("reset-scale", [=](std::shared_ptr<NetworkRequest> request) mutable {
    EditBufferUseCases useCase { editBuffer };
    useCase.resetScaleGroup();
  });

  addAction("set-macrocontrol-info", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto id = request->get("id");
    auto info = request->get("info");
    EditBufferUseCases useCase { editBuffer };
    if(auto mcUseCase = useCase.getMCUseCase(ParameterId { id }))
    {
      mcUseCase->setInfo(info);
    }
  });

  addAction("set-ribbon-touch-behaviour", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto id = request->get("id");
    Glib::ustring mode = request->get("mode");

    if(auto param = dynamic_cast<RibbonParameter*>(editBuffer->findParameterByID(ParameterId(id))))
    {
      RibbonParameterUseCases useCase(param);
      useCase.setTouchBehaviour(mode);
    }
  });

  addAction("set-ribbon-return-mode", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto id = request->get("id");
    auto mode = request->get("mode");

    if(auto param = dynamic_cast<RibbonParameter*>(editBuffer->findParameterByID(ParameterId(id))))
    {
      RibbonParameterUseCases useCase(param);
      useCase.setReturnMode(mode);
    }
  });

  addAction("set-pedal-mode", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto id = request->get("id");
    Glib::ustring mode = request->get("mode");

    if(auto param = dynamic_cast<PedalParameter*>(editBuffer->findParameterByID(ParameterId(id))))
    {
      PedalParameterUseCases useCase(param);
      useCase.setPedalMode(mode);
    }
  });

  addAction("reset-modulation", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto id = request->get("id");
    EditBufferUseCases ebUseCases(editBuffer);
    if(auto mcUseCase = ebUseCases.getMCUseCase(ParameterId { id }))
    {
      mcUseCase->resetConnectionsToTargets();
    }
  });

  addAction("randomize-sound", [=](const std::shared_ptr<NetworkRequest>&) mutable {
    SoundUseCases soundUseCases { editBuffer, editBuffer->getParent() };
    soundUseCases.randomizeSound();
  });

  addAction("init-sound", [=](const std::shared_ptr<NetworkRequest>&) mutable {
    SoundUseCases soundUseCases { editBuffer, editBuffer->getParent() };
    soundUseCases.initSound();
  });

  addAction("init-part", [=](const std::shared_ptr<NetworkRequest>& request) mutable {
    SoundUseCases soundUseCases { editBuffer, editBuffer->getParent() };
    soundUseCases.initPart(to<VoiceGroup>(request->get("part")));
  });

  addAction("rename-part", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto vg = to<VoiceGroup>(request->get("part"));
    auto name = request->get("name");
    SoundUseCases soundUseCases { editBuffer, editBuffer->getParent() };
    soundUseCases.renamePart(vg, name);
  });

  addAction("randomize-part", [=](std::shared_ptr<NetworkRequest> request) mutable {
    SoundUseCases soundUseCases { editBuffer, editBuffer->getParent() };
    soundUseCases.randomizePart(to<VoiceGroup>(request->get("part")));
  });

  addAction("mute", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto vg = to<VoiceGroup>(request->get("part"));
    EditBufferUseCases useCase(editBuffer);
    useCase.mutePart(vg);
  });

  addAction("unmute", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto vg = to<VoiceGroup>(request->get("part"));
    EditBufferUseCases ebUseCases(editBuffer);
    ebUseCases.unmutePart(vg);
  });

  addAction("mute-part-unmute-other", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto vg = to<VoiceGroup>(request->get("part"));
    EditBufferUseCases ebUseCases(editBuffer);
    ebUseCases.mutePartUnmuteOtherPart(vg);
  });

  addAction("set-modamount-and-value", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto id = request->get("id");
    auto modAmount = std::stod(request->get("mod-amount"));
    auto value = std::stod(request->get("value"));
    auto paramId = ParameterId(id);
    EditBufferUseCases useCase(editBuffer);
    useCase.setModAmountAndValue(paramId, modAmount, value);
  });

  addAction("set-modulation-limit", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto id = ParameterId { request->get("id") };
    auto newAmt = std::stod(request->get("mod-amt"));
    auto newParamVal = std::stod(request->get("param-val"));
    EditBufferUseCases ebUseCases(editBuffer);
    ebUseCases.setModulationLimit(id, newAmt, newParamVal);
  });

  addAction("unlock-all-groups", [=](std::shared_ptr<NetworkRequest> request) mutable {
    EditBufferUseCases ebUseCases(editBuffer);
    ebUseCases.unlockAllGroups();
  });

  addAction("lock-all-groups", [=](std::shared_ptr<NetworkRequest> request) mutable {
    EditBufferUseCases ebUseCases(editBuffer);
    ebUseCases.lockAllGroups();
  });

  addAction("toggle-group-lock", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto groupId = request->get("group");

    EditBufferUseCases ebUseCases(editBuffer);
    ebUseCases.toggleLock(groupId);
  });

  addAction("recall-current-from-preset", [=](auto request) {
    auto id = request->get("id");
    EditBufferUseCases ebUseCases(editBuffer);
    if(auto paramUseCase = ebUseCases.getUseCase(ParameterId { id }))
    {
      paramUseCase->recallParameterFromPreset();
    }
  });

  addAction("recall-mc-for-current-mod-param", [=](auto request) {
    auto id = request->get("id");
    EditBufferUseCases ebUseCases(editBuffer);
    if(auto modUseCase = ebUseCases.getModParamUseCase(ParameterId { id }))
      modUseCase->recallMCPos();
  });

  addAction("recall-mc-amount-for-current-mod-param", [=](auto request) {
    auto id = request->get("id");
    EditBufferUseCases ebUseCases(editBuffer);
    if(auto modUseCase = ebUseCases.getModParamUseCase(ParameterId { id }))
      modUseCase->recallMCAmount();
  });

  addAction("convert-to-single", [=](auto request) {
    auto voiceGroup = to<VoiceGroup>(request->get("voice-group"));
    SoundUseCases soundUseCases { editBuffer, editBuffer->getParent() };
    soundUseCases.convertToSingle(voiceGroup);
    //TODO move functionality someplace else -> hwui subscribe to eb->onConverted?
    //Application::get().getHWUI()->setCurrentVoiceGroupAndUpdateParameterSelection(transaction, VoiceGroup::I);
  });

  addAction("convert-to-split", [=](auto request) {
    SoundUseCases soundUseCases { editBuffer, editBuffer->getParent() };
    soundUseCases.convertToSplit();
  });

  addAction("convert-to-layer", [=](auto request) {
    SoundUseCases soundUseCases { editBuffer, editBuffer->getParent() };
    soundUseCases.convertToLayer();
  });

  addAction("load-selected-preset-part-into-editbuffer-part", [=](auto request) {
    auto presetPart = to<VoiceGroup>(request->get("preset-part"));
    auto editbufferPartPart = to<VoiceGroup>(request->get("editbuffer-part"));

    EditBufferUseCases ebUseCases(editBuffer);
    if(auto selectedPreset = editBuffer->getParent()->getSelectedPreset())
    {
      ebUseCases.undoableLoadToPart(selectedPreset, presetPart, editbufferPartPart);
    }
  });

  addAction("load-preset-part-into-editbuffer-part", [=](auto request) {
    auto presetPart = to<VoiceGroup>(request->get("preset-part"));
    auto loadTo = to<VoiceGroup>(request->get("editbuffer-part"));
    auto presetUUID = request->get("preset-uuid");
    auto pm = Application::get().getPresetManager();

    EditBufferUseCases ebUseCases(editBuffer);
    if(auto presetToLoad = pm->findPreset(Uuid { presetUUID }))
    {
      ebUseCases.undoableLoadToPart(presetToLoad, presetPart, loadTo);
    }
  });

  addAction("download-soled-as-png", [=](const std::shared_ptr<NetworkRequest>& request) {
    if(auto httpRequest = std::dynamic_pointer_cast<HTTPRequest>(request))
    {
      auto hwui = Application::get().getHWUI();
      httpRequest->respond(hwui->exportSoled());
    }
  });

  addAction("download-boled-as-png", [=](auto request) {
    if(auto httpRequest = std::dynamic_pointer_cast<HTTPRequest>(request))
    {
      auto hwui = Application::get().getHWUI();
      httpRequest->respond(hwui->exportBoled());
    }
  });

  addAction("select-part-from-webui", [=](auto request) {
    //TODO rethink what selected part really should be: datamodel? but not really as we have X foci for each webUI?
    auto part = to<VoiceGroup>(request->get("part"));
    auto hwui = Application::get().getHWUI();
    auto eb = Application::get().getPresetManager()->getEditBuffer();

    if(hwui->getCurrentVoiceGroup() != part && eb->isDual())
    {
      auto str = toString(part);
      auto scope = eb->getUndoScope().startTransaction("Select Part " + str);
      hwui->setCurrentVoiceGroupAndUpdateParameterSelection(scope->getTransaction(), part);
    }
  });

  addAction("default-param", [=](auto request) {
    auto id = ParameterId(request->get("id"));
    if(auto p = Application::get().getPresetManager()->getEditBuffer()->findParameterByID(id))
    {
      ParameterUseCases useCase(p);
      useCase.toggleLoadDefault();
    }
  });
}

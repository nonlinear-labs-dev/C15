#include "EditBufferActions.h"
#include "EditBuffer.h"
#include "parameters/MacroControlParameter.h"
#include <parameters/RibbonParameter.h>
#include <parameters/PedalParameter.h>
#include <parameters/ModulateableParameter.h>
#include <http/HTTPRequest.h>
#include <Application.h>
#include <device-settings/Settings.h>
#include <proxies/lpc/LPCProxy.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/audio-engine/AudioEngineProxy.h>
#include <groups/ParameterGroup.h>
#include <boost/algorithm/string.hpp>
#include <presets/PresetManager.h>
#include <http/UndoScope.h>
#include <parameters/names/ParameterDB.h>

//NonMember helperFunctions pre:
IntrusiveList<EditBufferActions::tParameterPtr> getScaleParameters(EditBuffer* editBuffer);

EditBufferActions::EditBufferActions(EditBuffer* editBuffer)
    : super("/presets/param-editor/")
{
  addAction("sync-audioengine", [=](std::shared_ptr<NetworkRequest> request) mutable {
    Application::get().getAudioEngineProxy()->sendEditBuffer();
  });

  addAction("select-param", [=](std::shared_ptr<NetworkRequest> request) mutable {
    Glib::ustring id = request->get("id");
    editBuffer->undoableSelectParameter(ParameterId(id));
  });

  addAction("set-param", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto id = request->get("id");
    auto value = std::stod(request->get("value"));
    editBuffer->setParameter(ParameterId(id), value);
  });

  addAction("set-mod-amount", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto amount = std::stod(request->get("amount"));
    editBuffer->setModulationAmount(amount);
  });

  addAction("set-mod-src", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto src = std::stoi(request->get("source"));
    editBuffer->setModulationSource(static_cast<MacroControls>(src));
  });

  addAction("reset", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto scope = editBuffer->getUndoScope().startTransaction("Init Sound");
    editBuffer->undoableInitSound(scope->getTransaction());
  });

  addAction("rename-mc", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto id = request->get("id");
    auto newName = request->get("new-name");

    if(auto param = dynamic_cast<MacroControlParameter*>(editBuffer->findParameterByID(ParameterId(id))))
      param->undoableSetGivenName(newName);
  });

  addAction("reset-scale", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto scope = editBuffer->getUndoScope().startTransaction("Reset Scale Group");

    for(auto scaleParam : getScaleParameters(editBuffer))
    {
      auto transaction = scope->getTransaction();
      scaleParam->reset(transaction, Initiator::EXPLICIT_WEBUI);
    }
  });

  addAction("set-macrocontrol-info", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto id = request->get("id");
    auto info = request->get("info");

    if(auto param = dynamic_cast<MacroControlParameter*>(editBuffer->findParameterByID(ParameterId(id))))
      param->undoableSetInfo(info);
  });

  addAction("set-ribbon-touch-behaviour", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto id = request->get("id");
    Glib::ustring mode = request->get("mode");

    if(auto param = dynamic_cast<RibbonParameter*>(editBuffer->findParameterByID(ParameterId(id))))
    {
      auto scope = editBuffer->getUndoScope().startTransaction("Set ribbon touch behaviour");
      param->undoableSetRibbonTouchBehaviour(scope->getTransaction(), mode);
    }
  });

  addAction("set-ribbon-return-mode", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto id = request->get("id");
    auto mode = request->get("mode");

    if(auto param = dynamic_cast<RibbonParameter*>(editBuffer->findParameterByID(ParameterId(id))))
    {
      auto scope = editBuffer->getUndoScope().startTransaction("Set ribbon return mode");
      param->undoableSetRibbonReturnMode(scope->getTransaction(), mode);
    }
  });

  addAction("set-pedal-mode", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto id = request->get("id");
    Glib::ustring mode = request->get("mode");

    if(auto param = dynamic_cast<PedalParameter*>(editBuffer->findParameterByID(ParameterId(id))))
    {
      auto scope = editBuffer->getUndoScope().startTransaction("Set pedal mode");
      param->undoableSetPedalMode(scope->getTransaction(), mode);
    }
  });

  addAction("reset-modulation", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto id = request->get("id");

    if(auto param = dynamic_cast<MacroControlParameter*>(editBuffer->findParameterByID(ParameterId(id))))
    {
      param->undoableResetConnectionsToTargets();
    }
  });

  addAction("randomize-sound", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto scope = editBuffer->getUndoScope().startTransaction("Randomize Sound");
    editBuffer->undoableRandomize(scope->getTransaction(), Initiator::EXPLICIT_WEBUI);
  });

  addAction("init-sound", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto scope = editBuffer->getUndoScope().startTransaction("Init Sound");
    editBuffer->undoableInitSound(scope->getTransaction());
  });

  addAction("init-part", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto vg = to<VoiceGroup>(request->get("part"));
    auto scope = editBuffer->getUndoScope().startTransaction("Init Part");
    editBuffer->undoableInitPart(scope->getTransaction(), vg);
  });

  addAction("rename-part", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto vg = to<VoiceGroup>(request->get("part"));
    auto name = request->get("name");
    auto scope = editBuffer->getUndoScope().startTransaction("Rename Part");
    editBuffer->setVoiceGroupName(scope->getTransaction(), name, vg);
  });

  addAction("randomize-part", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto vg = to<VoiceGroup>(request->get("part"));
    auto scope = editBuffer->getUndoScope().startTransaction("Randomize Part");
    editBuffer->undoableRandomizePart(scope->getTransaction(), vg, Initiator::EXPLICIT_WEBUI);
  });

  addAction("set-modamount-and-value", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto id = request->get("id");

    if(auto param = dynamic_cast<ModulateableParameter*>(editBuffer->findParameterByID(ParameterId(id))))
    {
      auto modAmount = std::stod(request->get("mod-amount"));
      auto value = std::stod(request->get("value"));

      auto scope = editBuffer->getUndoScope().startContinuousTransaction(
          param->getAmountCookie(), "Set Modulation Amount for '%0'", param->getGroupAndParameterName());
      auto transaction = scope->getTransaction();
      param->undoableSetModAmount(transaction, modAmount);
      param->setCPFromHwui(transaction, value);
    }
  });

  addAction("unlock-all-groups", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto scope = editBuffer->getUndoScope().startTransaction("Unlock all Groups");
    editBuffer->undoableUnlockAllGroups(scope->getTransaction());
  });

  addAction("lock-all-groups", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto scope = editBuffer->getUndoScope().startTransaction("Unlock all Groups");
    editBuffer->undoableLockAllGroups(scope->getTransaction());
  });

  addAction("toggle-group-lock", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto groupId = request->get("group");
    auto scope = editBuffer->getUndoScope().startTransaction("Toggle Group Lock");
    editBuffer->undoableToggleGroupLock(scope->getTransaction(), groupId);
  });

  addAction("recall-current-from-preset", [=](std::shared_ptr<NetworkRequest> request) {
    if(auto selParam = editBuffer->getSelected())
    {
      if(selParam->isChangedFromLoaded())
      {
        selParam->undoableRecallFromPreset();
      }
    }
  });

  addAction("recall-mc-for-current-mod-param", [=](auto request) {
    if(auto selParam = editBuffer->getSelected())
    {
      if(auto modP = dynamic_cast<ModulateableParameter*>(selParam))
      {
        modP->undoableRecallMCPos();
      }
    }
  });

  addAction("recall-mc-amount-for-current-mod-param", [=](auto request) {
    if(auto selParam = editBuffer->getSelected())
    {
      if(auto modP = dynamic_cast<ModulateableParameter*>(selParam))
      {
        modP->undoableRecallMCAmount();
      }
    }
  });

  addAction("convert-to-single", [=](auto request) {
    auto scope = editBuffer->getUndoScope().startTransaction("Convert to Single");
    auto transaction = scope->getTransaction();
    auto voiceGroup = to<VoiceGroup>(request->get("voice-group"));
    editBuffer->undoableConvertToSingle(transaction, voiceGroup);
    Application::get().getHWUI()->setFocusAndMode(FocusAndMode { UIFocus::Sound, UIMode::Select, UIDetail::Init });
  });

  addAction("convert-to-split", [=](auto request) {
    auto scope = editBuffer->getUndoScope().startTransaction("Convert to Split");
    auto transaction = scope->getTransaction();
    editBuffer->undoableConvertToDual(transaction, SoundType::Split);
    Application::get().getHWUI()->setFocusAndMode(FocusAndMode { UIFocus::Sound, UIMode::Select, UIDetail::Init });
  });

  addAction("convert-to-layer", [=](auto request) {
    auto scope = editBuffer->getUndoScope().startTransaction("Convert to Layer");
    auto transaction = scope->getTransaction();
    editBuffer->undoableConvertToDual(transaction, SoundType::Layer);
    Application::get().getHWUI()->setFocusAndMode(FocusAndMode { UIFocus::Sound, UIMode::Select, UIDetail::Init });
  });

  addAction("load-preset-part-into-editbuffer-part", [=](auto request) {
    auto presetPart = to<VoiceGroup>(request->get("preset-part"));
    auto editbufferPartPart = to<VoiceGroup>(request->get("editbuffer-part"));
    editBuffer->undoableLoadSelectedPresetPartIntoPart(presetPart, editbufferPartPart);
  });

  addAction("load-preset-into-editbuffer-part", [=](auto request) {
    auto presetUUID = request->get("uuid");
    auto loadInto = to<VoiceGroup>(request->get("load-to"));

    if(auto preset = editBuffer->getParent()->findPreset(presetUUID))
      editBuffer->undoableLoadSinglePreset(preset, loadInto);
  });
}

IntrusiveList<EditBufferActions::tParameterPtr> getScaleParameters(EditBuffer* editBuffer)
{
  auto paramGroup = editBuffer->getParameterGroupByID({ "Scale", VoiceGroup::Global });
  return paramGroup->getParameters();
}

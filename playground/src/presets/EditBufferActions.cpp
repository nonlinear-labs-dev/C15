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
#include <proxies/audio-engine/AudioEngineProxy.h>
#include <boost/algorithm/string.hpp>

//NonMember helperFunctions pre:
IntrusiveList<EditBufferActions::tParameterPtr> getScaleParameters(EditBuffer* editBuffer);

EditBufferActions::EditBufferActions(EditBuffer* editBuffer)
    : super("/presets/param-editor/")
{
  addAction("sync-lpc", [=](std::shared_ptr<NetworkRequest> request) mutable {
    Application::get().getSettings()->sendToLPC();
    Application::get().getLPCProxy()->sendEditBuffer();
    Application::get().getAudioEngineProxy()->sendEditBuffer();
  });

  addAction("select-param", [=](std::shared_ptr<NetworkRequest> request) mutable {
    Glib::ustring id = request->get("id");
    editBuffer->undoableSelectParameter(id);
  });

  addAction("set-param", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto id = std::stoi(request->get("id"));
    auto value = std::stod(request->get("value"));
    editBuffer->setParameter(id, value);
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
    Glib::ustring parameterId = request->get("id");
    Glib::ustring newName = request->get("new-name");

    size_t id = std::stoull(parameterId);

    if(auto param = dynamic_cast<MacroControlParameter*>(editBuffer->findParameterByID(id)))
    {
      param->undoableSetGivenName(newName);
    }
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
    Glib::ustring parameterId = request->get("id");
    Glib::ustring info = request->get("info");

    size_t id = std::stoull(parameterId);

    if(auto param = dynamic_cast<MacroControlParameter*>(editBuffer->findParameterByID(id)))
    {
      param->undoableSetInfo(info);
    }
  });

  addAction("import-reaktor-preset", [=](std::shared_ptr<NetworkRequest> request) mutable {
    Glib::ustring preset = request->get("preset");
    editBuffer->undoableImportReaktorPreset(preset);
  });

  addAction("download-editbuffer-as-text", [=](std::shared_ptr<NetworkRequest> request) mutable {
    if(auto httpRequest = std::dynamic_pointer_cast<HTTPRequest>(request))
    {
      auto stream = request->createStream("application/x-reaktor-table", false);
      auto quotedFileName = boost::replace_all_copy(editBuffer->getName().raw(), "\"", "'");
      httpRequest->setHeader("Content-Disposition", "attachment; filename=\"" + quotedFileName + ".rvt\"");

      Glib::ustring preset = editBuffer->exportReaktorPreset();
      httpRequest->respond(boost::replace_all_copy(preset.raw(), "\n", "\r\n"));
    }
  });

  addAction("open-editbuffer-as-text", [=](std::shared_ptr<NetworkRequest> request) mutable {
    if(auto httpRequest = std::dynamic_pointer_cast<HTTPRequest>(request))
    {
      auto stream = request->createStream("text/plain", false);
      Glib::ustring preset = editBuffer->exportReaktorPreset();
      httpRequest->respond(boost::replace_all_copy(preset.raw(), "\n", "\r\n"));
    }
  });

  addAction("set-ribbon-touch-behaviour", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto parameterId = std::stoi(request->get("id"));
    Glib::ustring mode = request->get("mode");

    if(auto param = dynamic_cast<RibbonParameter*>(editBuffer->findParameterByID(parameterId)))
    {
      auto scope = editBuffer->getUndoScope().startTransaction("Set ribbon touch behaviour");
      param->undoableSetRibbonTouchBehaviour(scope->getTransaction(), mode);
    }
  });

  addAction("set-ribbon-return-mode", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto parameterId = std::stoi(request->get("id"));
    Glib::ustring mode = request->get("mode");

    if(auto param = dynamic_cast<RibbonParameter*>(editBuffer->findParameterByID(parameterId)))
    {
      auto scope = editBuffer->getUndoScope().startTransaction("Set ribbon return mode");
      param->undoableSetRibbonReturnMode(scope->getTransaction(), mode);
    }
  });

  addAction("set-pedal-mode", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto parameterId = std::stoi(request->get("id"));
    Glib::ustring mode = request->get("mode");

    if(auto param = dynamic_cast<PedalParameter*>(editBuffer->findParameterByID(parameterId)))
    {
      auto scope = editBuffer->getUndoScope().startTransaction("Set pedal mode");
      param->undoableSetPedalMode(scope->getTransaction(), mode);
    }
  });

  addAction("reset-modulation", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto parameterId = std::stoi(request->get("id"));

    if(auto param = dynamic_cast<MacroControlParameter*>(editBuffer->findParameterByID(parameterId)))
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

  addAction("set-modamount-and-value", [=](std::shared_ptr<NetworkRequest> request) mutable {
    auto id = std::stoi(request->get("id"));

    if(auto param = dynamic_cast<ModulateableParameter*>(editBuffer->findParameterByID(id)))
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
}

IntrusiveList<EditBufferActions::tParameterPtr> getScaleParameters(EditBuffer* editBuffer)
{
  auto paramGroup = editBuffer->getParameterGroupByID("Scale");
  return paramGroup->getParameters();
}

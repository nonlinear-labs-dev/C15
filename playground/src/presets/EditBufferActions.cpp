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
#include <boost/algorithm/string.hpp>

EditBufferActions::EditBufferActions(shared_ptr<EditBuffer> editBuffer)
    : super("/presets/param-editor/")
{
  addAction("sync-lpc", [=](shared_ptr<NetworkRequest> request) mutable {
    Application::get().getSettings()->sendToLPC();
    Application::get().getLPCProxy()->sendEditBuffer();
  });

  addAction("select-param", [=](shared_ptr<NetworkRequest> request) mutable {
    Glib::ustring id = request->get("id");
    editBuffer->undoableSelectParameter(id);
  });

  addAction("set-param", [=](shared_ptr<NetworkRequest> request) mutable {
    auto id = stoi(request->get("id"));
    auto value = stod(request->get("value"));
    editBuffer->setParameter(id, value);
  });

  addAction("set-mod-amount", [=](shared_ptr<NetworkRequest> request) mutable {
    auto amount = stod(request->get("amount"));
    editBuffer->setModulationAmount(amount);
  });

  addAction("set-mod-src", [=](shared_ptr<NetworkRequest> request) mutable {
    auto src = stoi(request->get("source"));
    editBuffer->setModulationSource(src);
  });

  addAction("reset", [=](shared_ptr<NetworkRequest> request) mutable {
    auto scope = editBuffer->getUndoScope().startTransaction("Init Sound");
    editBuffer->undoableInitSound(scope->getTransaction());
  });

  addAction("rename-mc", [=](shared_ptr<NetworkRequest> request) mutable {
    Glib::ustring parameterId = request->get("id");
    Glib::ustring newName = request->get("new-name");

    size_t id = stoull(parameterId);

    if(auto param = dynamic_cast<MacroControlParameter*>(editBuffer->findParameterByID(id)))
    {
      param->undoableSetGivenName(newName);
    }
  });

  addAction("set-macrocontrol-info", [=](shared_ptr<NetworkRequest> request) mutable {
    Glib::ustring parameterId = request->get("id");
    Glib::ustring info = request->get("info");

    size_t id = stoull(parameterId);

    if(auto param = dynamic_cast<MacroControlParameter*>(editBuffer->findParameterByID(id)))
    {
      param->undoableSetInfo(info);
    }
  });

  addAction("import-reaktor-preset", [=](shared_ptr<NetworkRequest> request) mutable {
    Glib::ustring preset = request->get("preset");
    editBuffer->undoableImportReaktorPreset(preset);
  });

  addAction("download-editbuffer-as-text", [=](shared_ptr<NetworkRequest> request) mutable {
    if(auto httpRequest = dynamic_pointer_cast<HTTPRequest>(request))
    {
      auto stream = request->createStream("application/x-reaktor-table", false);
      auto quotedFileName = boost::replace_all_copy(editBuffer->getName().raw(), "\"", "'");
      httpRequest->setHeader("Content-Disposition", "attachment; filename=\"" + quotedFileName + ".rvt\"");

      Glib::ustring preset = editBuffer->exportReaktorPreset();
      httpRequest->respond(boost::replace_all_copy(preset.raw(), "\n", "\r\n"));
    }
  });

  addAction("open-editbuffer-as-text", [=](shared_ptr<NetworkRequest> request) mutable {
    if(auto httpRequest = dynamic_pointer_cast<HTTPRequest>(request))
    {
      auto stream = request->createStream("text/plain", false);
      Glib::ustring preset = editBuffer->exportReaktorPreset();
      httpRequest->respond(boost::replace_all_copy(preset.raw(), "\n", "\r\n"));
    }
  });

  addAction("set-ribbon-touch-behaviour", [=](shared_ptr<NetworkRequest> request) mutable {
    auto parameterId = stoi(request->get("id"));
    Glib::ustring mode = request->get("mode");

    if(auto param = dynamic_cast<RibbonParameter*>(editBuffer->findParameterByID(parameterId)))
    {
      auto scope = editBuffer->getUndoScope().startTransaction("Set ribbon touch behaviour");
      param->undoableSetRibbonTouchBehaviour(scope->getTransaction(), mode);
    }
  });

  addAction("set-ribbon-return-mode", [=](shared_ptr<NetworkRequest> request) mutable {
    auto parameterId = stoi(request->get("id"));
    Glib::ustring mode = request->get("mode");

    if(auto param = dynamic_cast<RibbonParameter*>(editBuffer->findParameterByID(parameterId)))
    {
      auto scope = editBuffer->getUndoScope().startTransaction("Set ribbon return mode");
      param->undoableSetRibbonReturnMode(scope->getTransaction(), mode);
    }
  });

  addAction("set-pedal-mode", [=](shared_ptr<NetworkRequest> request) mutable {
    auto parameterId = stoi(request->get("id"));
    Glib::ustring mode = request->get("mode");

    if(auto param = dynamic_cast<PedalParameter*>(editBuffer->findParameterByID(parameterId)))
    {
      auto scope = editBuffer->getUndoScope().startTransaction("Set pedal mode");
      param->undoableSetPedalMode(scope->getTransaction(), mode);
    }
  });

  addAction("reset-modulation", [=](shared_ptr<NetworkRequest> request) mutable {
    auto parameterId = stoi(request->get("id"));

    if(auto param = dynamic_cast<MacroControlParameter*>(editBuffer->findParameterByID(parameterId)))
    {
      param->undoableResetConnectionsToTargets();
    }
  });

  addAction("randomize-sound", [=](shared_ptr<NetworkRequest> request) mutable {
    auto scope = editBuffer->getUndoScope().startTransaction("Randomize Sound");
    editBuffer->undoableRandomize(scope->getTransaction(), Initiator::EXPLICIT_WEBUI);
  });

  addAction("init-sound", [=](shared_ptr<NetworkRequest> request) mutable {
    auto scope = editBuffer->getUndoScope().startTransaction("Init Sound");
    editBuffer->undoableInitSound(scope->getTransaction());
  });

  addAction("set-modamount-and-value", [=](shared_ptr<NetworkRequest> request) mutable {
    auto id = stoi(request->get("id"));

    if(auto param = dynamic_cast<ModulateableParameter*>(editBuffer->findParameterByID(id)))
    {
      auto modAmount = stod(request->get("mod-amount"));
      auto value = stod(request->get("value"));

      auto scope = editBuffer->getUndoScope().startContinuousTransaction(
          param->getAmountCookie(), "Set Modulation Amount for '%0'", param->getGroupAndParameterName());
      auto transaction = scope->getTransaction();
      param->undoableSetModAmount(transaction, modAmount);
      param->setCPFromHwui(transaction, value);
    }
  });

  addAction("unlock-all-groups", [=](shared_ptr<NetworkRequest> request) mutable {
    auto scope = editBuffer->getUndoScope().startTransaction("Unlock all Groups");
    editBuffer->undoableUnlockAllGroups(scope->getTransaction());
  });

  addAction("lock-all-groups", [=](shared_ptr<NetworkRequest> request) mutable {
    auto scope = editBuffer->getUndoScope().startTransaction("Unlock all Groups");
    editBuffer->undoableLockAllGroups(scope->getTransaction());
  });

  addAction("toggle-group-lock", [=](shared_ptr<NetworkRequest> request) mutable {
    auto groupId = request->get("group");
    auto scope = editBuffer->getUndoScope().startTransaction("Toggle Group Lock");
    editBuffer->undoableToggleGroupLock(scope->getTransaction(), groupId);
  });
}

EditBufferActions::~EditBufferActions()
{
}

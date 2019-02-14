#include <Application.h>
#include <http/HTTPRequest.h>
#include <presets/EditBuffer.h>
#include <presets/Bank.h>
#include <presets/Preset.h>
#include <presets/PresetManager.h>
#include <presets/PresetManagerActions.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/panel-unit/boled/BOLED.h>
#include <proxies/hwui/panel-unit/boled/SplashLayout.h>
#include <proxies/hwui/panel-unit/EditPanel.h>
#include <proxies/hwui/panel-unit/PanelUnit.h>
#include <serialization/PresetManagerSerializer.h>
#include <serialization/PresetSerializer.h>
#include <serialization/EditBufferSerializer.h>
#include <tools/Uuid.h>
#include <xml/MemoryInStream.h>
#include <xml/OutStream.h>
#include <xml/VersionAttribute.h>
#include <xml/XmlReader.h>
#include <xml/XmlWriter.h>
#include <boost/algorithm/string.hpp>
#include <tools/StringTools.h>
#include <device-settings/Settings.h>
#include <device-settings/AutoLoadSelectedPreset.h>
#include <proxies/lpc/LPCProxy.h>
#include <proxies/lpc/LPCParameterChangeSurpressor.h>
#include <tools/TimeTools.h>
#include <proxies/hwui/panel-unit/boled/setup/ExportBackupEditor.h>
#include "SearchQuery.h"
#include <device-settings/DebugLevel.h>

PresetManagerActions::PresetManagerActions(PresetManager &presetManager)
    : RPCActionManager("/presets/")
    , m_presetManager(presetManager)
{
  addAction("new-bank", [&](shared_ptr<NetworkRequest> request) mutable {
    auto x = request->get("x");
    auto y = request->get("y");
    auto name = request->get("name");
    auto scope = presetManager.getUndoScope().startTransaction("New Bank");
    auto transaction = scope->getTransaction();
    auto bank = presetManager.addBank(transaction);
    bank->setX(transaction, x);
    bank->setY(transaction, y);
    bank->setName(scope->getTransaction(), name);
    presetManager.selectBank(scope->getTransaction(), bank->getUuid());
  });

  addAction("new-bank-from-edit-buffer", [&](shared_ptr<NetworkRequest> request) mutable {
    Glib::ustring x = request->get("x");
    Glib::ustring y = request->get("y");

    auto scope = presetManager.getUndoScope().startTransaction("New Bank");
    auto transaction = scope->getTransaction();
    auto bank = presetManager.addBank(transaction);
    bank->setX(transaction, x);
    bank->setY(transaction, y);
    auto preset = bank->appendPreset(transaction);
    preset->copyFrom(transaction, presetManager.getEditBuffer());
    bank->selectPreset(transaction, preset->getUuid());
    presetManager.selectBank(transaction, bank->getUuid());
    presetManager.getEditBuffer()->undoableUpdateLoadedPresetInfo(transaction);
  });

  addAction("rename-bank", [&](shared_ptr<NetworkRequest> request) mutable {
    auto uuid = request->get("uuid");
    auto newName = request->get("name");

    if(auto bank = presetManager.findBank(uuid))
    {
      auto &undoScope = presetManager.getUndoScope();
      auto transactionScope = undoScope.startTransaction("Rename Bank '%0' to '%1'", bank->getName(true), newName);
      auto transaction = transactionScope->getTransaction();
      bank->setName(transaction, newName);
      presetManager.getEditBuffer()->undoableUpdateLoadedPresetInfo(transaction);
    }
  });

  addAction("select-bank", [&](shared_ptr<NetworkRequest> request) mutable {
    auto uuid = request->get("uuid");

    if(auto bank = presetManager.findBank(uuid))
    {
      auto &undoScope = presetManager.getUndoScope();
      auto transactionScope = undoScope.startTransaction("Select Bank '%0'", bank->getName(true));
      auto transaction = transactionScope->getTransaction();
      presetManager.selectBank(transaction, uuid);
    }
  });

  addAction("delete-bank", [&](shared_ptr<NetworkRequest> request) mutable {
    auto uuid = request->get("uuid");

    if(auto bank = presetManager.findBank(uuid))
    {
      auto scope = presetManager.getUndoScope().startTransaction("Delete Bank '%0'", bank->getName(true));
      auto transaction = scope->getTransaction();

      if(presetManager.getSelectedBankUuid() == uuid)
        if(!presetManager.selectPreviousBank(transaction))
          presetManager.selectNextBank(transaction);

      presetManager.deleteBank(transaction, uuid);
    }
  });

  addAction("find-unique-preset-name", [&](shared_ptr<NetworkRequest> request) mutable {
    if(auto httpRequest = dynamic_pointer_cast<HTTPRequest>(request))
    {
      Glib::ustring basedOn = request->get("based-on");
      Glib::ustring result = presetManager.createPresetNameBasedOn(basedOn);
      httpRequest->respond(result);
    }
  });

  addAction("store-init", [&](shared_ptr<NetworkRequest> request) mutable {
    auto scope = presetManager.getUndoScope().startTransaction("Store Init");
    auto transaction = scope->getTransaction();
    presetManager.storeInitSound(transaction);
  });

  addAction("reset-init", [&](shared_ptr<NetworkRequest> request) mutable {
    auto scope = presetManager.getUndoScope().startTransaction("Reset Init");
    auto transaction = scope->getTransaction();
    presetManager.resetInitSound(transaction);
  });

  addAction("import-all-banks", [&](shared_ptr<NetworkRequest> request) mutable {
    if(auto http = dynamic_pointer_cast<HTTPRequest>(request))
    {
      auto &boled = Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled();
      auto scope = presetManager.getUndoScope().startTransaction("Import all Banks");
      auto transaction = scope->getTransaction();

      boled.setOverlay(new SplashLayout());
      auto *buffer = http->getFlattenedBuffer();

      handleImportBackupFile(transaction, buffer, http);

      boled.resetOverlay();
      soup_buffer_free(buffer);
    }
  });

  addAction("load-preset-from-xml", [&](shared_ptr<NetworkRequest> request) mutable {
    if(auto http = dynamic_pointer_cast<HTTPRequest>(request))
    {
      auto scope = presetManager.getUndoScope().startTransaction("Load Compare Buffer");
      auto transaction = scope->getTransaction();
      auto xml = http->get("xml", "");

      MemoryInStream stream(xml, false);
      XmlReader reader(stream, transaction);
      auto editBuffer = presetManager.getEditBuffer();

      auto newPreset = std::make_shared<Preset>(editBuffer);

      if(!reader.read<PresetSerializer>(newPreset.get()))
      {
        transaction->rollBack();
        http->respond("Invalid File!");
        DebugLevel::warning("Could not read Preset xml!");
        return;
      }

      LPCParameterChangeSurpressor lpcParameterChangeSupressor(transaction);
      auto autoLoadSetting = Application::get().getSettings()->getSetting<AutoLoadSelectedPreset>();
      auto scopedLock = autoLoadSetting->scopedOverlay(BooleanSettings::BOOLEAN_SETTING_FALSE);
      editBuffer->copyFrom(transaction, newPreset.get());
      editBuffer->undoableSetLoadedPresetInfo(transaction, newPreset.get());
    }
  });

  addAction("move-cluster", [&](shared_ptr<NetworkRequest> request) mutable {
    auto scope = presetManager.getUndoScope().startTransaction("Moved Banks");
    auto transaction = scope->getTransaction();

    std::vector<std::string> values;

    auto csv = request->get("csv");
    boost::split(values, csv, boost::is_any_of(","));

    assert(values.size() % 3 == 0);

    for(auto i = values.begin(); i != values.end();)
    {
      if(auto selBank = presetManager.findBank(*(i++)))
      {
        auto x = *(i++);
        auto y = *(i++);
        selBank->setX(transaction, x);
        selBank->setY(transaction, y);
      }
      else
      {
        std::advance(i, 2);
      }
    }
  });
}

PresetManagerActions::~PresetManagerActions()
{
}

void PresetManagerActions::handleImportBackupFile(UNDO::Transaction *transaction, SoupBuffer *buffer,
                                                  shared_ptr<HTTPRequest> http)
{
  if(auto lock = m_presetManager.getLoadingLock())
  {
    m_presetManager.clear(transaction);

    MemoryInStream stream(buffer, true);
    XmlReader reader(stream, transaction);

    if(!reader.read<PresetManagerSerializer>(&m_presetManager))
    {
      transaction->rollBack();
      http->respond("Invalid File. Please choose correct xml.tar.gz or xml.zip file.");
    }
    else
    {
      m_presetManager.getEditBuffer()->sendToLPC();
    }
  }
}

bool PresetManagerActions::handleRequest(const Glib::ustring &path, shared_ptr<NetworkRequest> request)
{
  if(super::handleRequest(path, request))
    return true;

  if(path.find("/presets/search-preset") == 0)
  {
    if(auto httpRequest = dynamic_pointer_cast<HTTPRequest>(request))
    {
      Glib::ustring query = request->get("query");
      Glib::ustring mode = request->get("combine");
      Glib::ustring field = request->get("fields");

      std::vector<SearchQuery::Fields> fields;

      auto splitFieldStrings = StringTools::splitStringOnAnyDelimiter(field, ',');
      std::for_each(splitFieldStrings.begin(), splitFieldStrings.end(), [&](const std::string &t) {
        if(t == "name")
          fields.push_back(SearchQuery::Fields::Name);
        else if(t == "comment")
          fields.push_back(SearchQuery::Fields::Comment);
        else if(t == "devicename")
          fields.push_back(SearchQuery::Fields::DeviceName);
      });

      auto stream = request->createStream("text/xml", false);
      XmlWriter writer(stream);
      Application::get().getPresetManager()->searchPresets(writer, query, mode, std::move(fields));
      return true;
    }
  }

  if(path.find("/presets/download-banks") == 0)
  {
    if(auto httpRequest = dynamic_pointer_cast<HTTPRequest>(request))
    {
      auto &boled = Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled();
      boled.setOverlay(new SplashLayout());

      const auto time = TimeTools::getDisplayStringFromStamp(TimeTools::getAdjustedTimestamp());
      const auto timeWithoutWhitespaces = StringTools::replaceAll(time, " ", "-");
      const auto timeSanitized = StringTools::replaceAll(timeWithoutWhitespaces, ":", "-");
      auto stream = request->createStream("application/zip", true);
      httpRequest->setHeader("Content-Disposition",
                             "attachment; filename=\"" + timeSanitized + "-nonlinear-c15-banks.xml.tar.gz\"");
      ExportBackupEditor::writeBackupToStream(stream);

      boled.resetOverlay();
      return true;
    }
  }

  if(path.find("/presets/get-diff") == 0)
  {
    if(auto httpRequest = dynamic_pointer_cast<HTTPRequest>(request))
    {
      auto pm = Application::get().getPresetManager();
      auto eb = pm->getEditBuffer();
      auto ebAsPreset = std::make_unique<Preset>(pm, *eb);
      auto aUUID = request->get("p1");
      auto bUUID = request->get("p2");
      auto a = pm->findPreset(aUUID);
      auto b = pm->findPreset(bUUID);
      a = a ? a : ebAsPreset.get();
      b = b ? b : ebAsPreset.get();
      auto stream = request->createStream("text/xml", false);
      XmlWriter writer(stream);
      a->writeDiff(writer, b);
    }
  }

  return false;
}

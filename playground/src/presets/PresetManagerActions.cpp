#include <Application.h>
#include <http/HTTPRequest.h>
#include <presets/EditBuffer.h>
#include <presets/PresetBank.h>
#include <presets/PresetManager.h>
#include <presets/PresetManagerActions.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/panel-unit/boled/BOLED.h>
#include <proxies/hwui/panel-unit/boled/SplashLayout.h>
#include <proxies/hwui/panel-unit/EditPanel.h>
#include <proxies/hwui/panel-unit/PanelUnit.h>
#include <serialization/PresetManagerSerializer.h>
#include <serialization/PresetSerializer.h>
#include <tools/Uuid.h>
#include <xml/MemoryInStream.h>
#include <xml/OutStream.h>
#include <xml/VersionAttribute.h>
#include <xml/XmlReader.h>
#include <xml/XmlWriter.h>
#include <boost/algorithm/string.hpp>
#include <tools/StringTools.h>

PresetManagerActions::PresetManagerActions(PresetManager &presetManager) :
    RPCActionManager("/presets/"),
    m_presetManager(presetManager)
{
  addAction("new-bank", [&](shared_ptr<NetworkRequest> request) mutable
  {
    Glib::ustring x = request->get("x");
    Glib::ustring y = request->get("y");
    Glib::ustring name = request->get("name");

    UNDO::Scope::tTransactionScopePtr scope = presetManager.getUndoScope().startTransaction("New Bank");
    auto bank = presetManager.addBank(scope->getTransaction(), x, y);
    bank->undoableSetName(scope->getTransaction(), name);
    presetManager.undoableSelectBank(scope->getTransaction(), bank->getUuid());
  });

  addAction("new-bank-from-edit-buffer", [&](shared_ptr<NetworkRequest> request) mutable
  {
    Glib::ustring x = request->get("x");
    Glib::ustring y = request->get("y");

    UNDO::Scope::tTransactionScopePtr scope = presetManager.getUndoScope().startTransaction("New Bank");
    UNDO::Scope::tTransactionPtr transaction = scope->getTransaction();
    auto bank = presetManager.addBank(transaction, x, y);

    bank->undoableInsertPreset(transaction, 0);
    bank->undoableOverwritePreset(transaction, 0, static_pointer_cast<Preset>(presetManager.getEditBuffer()));
    presetManager.undoableSelectBank(transaction, bank->getUuid());
    bank->getPreset(0)->undoableSelect(transaction);

    presetManager.getEditBuffer()->undoableUpdateLoadedPresetInfo(transaction);
  });

  addAction("rename-bank",
      [&](shared_ptr<NetworkRequest> request) mutable
      {
        Glib::ustring uuid = request->get("uuid");
        Glib::ustring newName = request->get("name");

        if(tBankPtr bank = presetManager.findBank(uuid))
        {
          UNDO::Scope::tTransactionScopePtr scope = presetManager.getUndoScope().startTransaction("Rename Preset Bank '%0' to '%1'", bank->getName(true), newName);
          UNDO::Scope::tTransactionPtr transaction = scope->getTransaction();
          bank->undoableSetName(transaction, newName);
          presetManager.undoableSelectBank(scope->getTransaction(), bank->getUuid());
          presetManager.getEditBuffer()->undoableUpdateLoadedPresetInfo(scope->getTransaction());
        }
      });

  addAction("select-bank", [&](shared_ptr<NetworkRequest> request) mutable
  {
    Glib::ustring uuid = request->get("uuid");
    presetManager.undoableSelectBank(uuid);
  });

  addAction("delete-bank", [&](shared_ptr<NetworkRequest> request) mutable
  {
    Glib::ustring uuid = request->get("uuid");

    if(tBankPtr bank = presetManager.findBank(uuid))
    {
      bool wasSelected = presetManager.getSelectedBank() == bank;
      UNDO::Scope::tTransactionScopePtr scope = presetManager.getUndoScope().startTransaction("Delete Bank '%0'", bank->getName(true));
      auto transaction = scope->getTransaction();
      presetManager.undoableDeleteBank(transaction, bank);
    }
  });

  addAction("find-unique-preset-name", [&](shared_ptr<NetworkRequest> request) mutable
  {
    if (auto httpRequest = dynamic_pointer_cast<HTTPRequest> (request))
    {
      Glib::ustring basedOn = request->get("based-on");
      Glib::ustring result = presetManager.createPresetNameBasedOn (basedOn);
      httpRequest->respond (result);
    }
  });

  addAction("store-init", [&](shared_ptr<NetworkRequest> request) mutable
  {
    UNDO::Scope::tTransactionScopePtr scope = presetManager.getUndoScope().startTransaction("Store Init");
    auto transaction = scope->getTransaction();
    presetManager.undoableStoreInitSound(transaction);
  });

  addAction("reset-init", [&](shared_ptr<NetworkRequest> request) mutable
  {
    UNDO::Scope::tTransactionScopePtr scope = presetManager.getUndoScope().startTransaction("Reset Init");
    auto transaction = scope->getTransaction();
    presetManager.undoableResetInitSound(transaction);
  });

  addAction("import-all-banks", [&] (shared_ptr<NetworkRequest> request) mutable
  {
    if(auto http = dynamic_pointer_cast<HTTPRequest>(request))
    {
      auto &boled = Application::get().getHWUI ()->getPanelUnit ().getEditPanel ().getBoled ();
      UNDO::Scope::tTransactionScopePtr scope = presetManager.getUndoScope().startTransaction ("Import all Banks");
      auto transaction = scope->getTransaction();

      boled.setOverlay (new SplashLayout ());
      auto* buffer = http->getFlattenedBuffer();

      handleImportBackupFile(transaction, buffer, http);

      boled.resetOverlay();
      soup_buffer_free(buffer);
    }
  });

  addAction("load-editbuffer", [&] (shared_ptr<NetworkRequest> request) mutable
  {
    if(auto http = dynamic_pointer_cast<HTTPRequest>(request))
    {
      UNDO::Scope::tTransactionScopePtr scope = presetManager.getUndoScope().startTransaction ("Load Edit Buffer");
      auto transaction = scope->getTransaction();
      auto xml = http->get("xml", "");

      MemoryInStream stream(xml, false);
      XmlReader reader(stream, transaction);

      if(!reader.read<PresetSerializer>(presetManager.getEditBuffer().get()))
      {
        transaction->rollBack();
        http->respond("Invalid File. Please choose correct xml.tar.gz or xml.zip file.");
      }

      m_presetManager.getEditBuffer()->sendToLPC();
    }
  });

  addAction("move-cluster", [&] (shared_ptr<NetworkRequest> request) mutable
  {
    UNDO::Scope::tTransactionScopePtr scope = presetManager.getUndoScope().startTransaction ("Moved Banks");
    auto transaction = scope->getTransaction();

    std::vector<std::string> values;

    auto csv = request->get("csv");
    boost::split(values, csv, boost::is_any_of(","));

    g_assert(values.size() % 3 == 0);

    for(auto i = values.begin(); i != values.end();)
    {
      if(auto selBank = presetManager.findBank(*(i++)))
      {
        auto x = *(i++);
        auto y = *(i++);
        selBank->undoableSetPosition(transaction, x, y);
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

void PresetManagerActions::handleImportBackupFile(UNDO::TransactionCreationScope::tTransactionPtr transaction, SoupBuffer* buffer,
                                                  shared_ptr<HTTPRequest> http)
{
  m_presetManager.undoableClear(transaction);

  MemoryInStream stream(buffer, true);
  XmlReader reader(stream, transaction);

  if(!reader.read<PresetManagerSerializer>(std::ref(m_presetManager)))
  {
    transaction->rollBack();
    http->respond("Invalid File. Please choose correct xml.tar.gz or xml.zip file.");
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

      std::vector<PresetManager::presetInfoSearchFields> fields;

      auto splitFieldStrings = StringTools::splitStringOnAnyDelimiter(field, ',');
      std::for_each(splitFieldStrings.begin(), splitFieldStrings.end(), [&](std::string t)
      {
        if(t == "name")
        {
          fields.push_back(PresetManager::presetInfoSearchFields::name);
        }
        else if(t == "comment")
        {
          fields.push_back(PresetManager::presetInfoSearchFields::comment);
        }
        else if(t == "devicename")
        {
          fields.push_back(PresetManager::presetInfoSearchFields::devicename);
        }
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

      auto stream = request->createStream("application/zip", true);
      httpRequest->setHeader("Content-Disposition", "attachment; filename=\"nonlinear-c15-banks.xml.tar.gz\"");
      XmlWriter writer(stream);
      auto pm = Application::get().getPresetManager();
      PresetManagerSerializer serializer(*pm.get());
      serializer.write(writer, VersionAttribute::get());
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
      auto a = pm->findPreset(request->get("p1"));
      auto b = pm->findPreset(request->get("p2"));

      auto preset1 = a ? a : eb;
      auto preset2 = b ? b : eb;

      if(preset1 && preset2)
      {
        auto stream = request->createStream("text/xml", false);
        XmlWriter writer(stream);
        preset1->writeDiff(writer, preset2.get());
        return true;
      }
    }
  }

  return false;
}

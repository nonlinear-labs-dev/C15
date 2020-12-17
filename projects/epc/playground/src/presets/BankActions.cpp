#include "BankActions.h"

#include <serialization/EditBufferSerializer.h>
#include <serialization/PresetBankSerializer.h>
#include <serialization/PresetSerializer.h>
#include <presets/PresetManager.h>
#include <presets/Bank.h>
#include <presets/Preset.h>
#include "EditBuffer.h"
#include "http/SoupOutStream.h"
#include <http/HTTPRequest.h>
#include "xml/XmlWriter.h"
#include "ClusterEnforcement.h"
#include <xml/MemoryInStream.h>
#include <xml/XmlReader.h>
#include <device-settings/DirectLoadSetting.h>
#include <device-settings/Settings.h>
#include <device-info/DateTimeInfo.h>
#include <Application.h>
#include <tools/PerformanceTimer.h>
#include <xml/VersionAttribute.h>
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <tools/TimeTools.h>
#include <proxies/hwui/HWUI.h>
#include <nltools/Assert.h>
#include <use-cases/PresetUseCases.h>

BankActions::BankActions(PresetManager &presetManager)
    : RPCActionManager("/presets/banks/")
    , m_presetManager(presetManager)
{
  addAction("drop-presets-above", [&](std::shared_ptr<NetworkRequest> request) {
    Glib::ustring csv = request->get("presets");
    Glib::ustring anchorUUID = request->get("anchor");

    PresetManagerUseCases useCase(&m_presetManager);
    useCase.dropPresets(anchorUUID, PresetManagerUseCases::DropActions::Above, csv);
  });

  addAction("drop-presets-below", [&](std::shared_ptr<NetworkRequest> request) {
    Glib::ustring csv = request->get("presets");
    Glib::ustring anchorUUID = request->get("anchor");

    PresetManagerUseCases useCase(&m_presetManager);
    useCase.dropPresets(anchorUUID, PresetManagerUseCases::DropActions::Below, csv);
  });

  addAction("drop-presets-to", [&](std::shared_ptr<NetworkRequest> request) {
    Glib::ustring csv = request->get("presets");
    Glib::ustring anchorUUID = request->get("anchor");

    PresetManagerUseCases useCase(&m_presetManager);
    useCase.dropPresets(anchorUUID, PresetManagerUseCases::DropActions::Onto, csv);
  });

  addAction("rename-preset", [&](std::shared_ptr<NetworkRequest> request) {
    Glib::ustring uuid = request->get("uuid");
    Glib::ustring newName = request->get("name");

    if(auto p = m_presetManager.findPreset(uuid))
    {
      PresetUseCases useCase(p);
      useCase.rename(newName);
    }
  });

  addAction("move-preset-above", [&](std::shared_ptr<NetworkRequest> request) mutable {
    auto presetToMoveUuid = request->get("presetToMove");
    auto presetAnchorUuid = request->get("anchor");


    PresetManagerUseCases useCase(&m_presetManager);
	// move into method
Bank::movePresetBetweenBanks(transaction, toMove, anchor);
    useCase.movePresetAbove(presetToMoveUuid, presetAnchorUuid);
  });

  addAction("move-preset-below", [&](std::shared_ptr<NetworkRequest> request) {
    auto presetToMoveUuid = request->get("presetToMove");
    auto presetAnchorUuid = request->get("anchor");

    PresetManagerUseCases useCases(&m_presetManager);
        Bank::movePresetBetweenBanks(transaction, toMove, anchor);
    useCases.movePresetBelow(presetToMoveUuid, presetAnchorUuid);
  });

  addAction("move-preset-to", [&](std::shared_ptr<NetworkRequest> request) {
    auto presetToOverwrite = request->get("presetToOverwrite");
    auto overwriteWith = request->get("overwriteWith");

        Bank::movePresetBetweenBanks(transaction, srcPreset, anchor);
    PresetManagerUseCases useCases(&m_presetManager);
    useCases.movePresetTo(overwriteWith, presetToOverwrite);
  });

  addAction("overwrite-preset", [&](std::shared_ptr<NetworkRequest> request) {
    auto presetToOverwrite = request->get("presetToOverwrite");
    auto overwriteWith = request->get("overwriteWith");

    auto sourcePreset = m_presetManager.findPreset(overwriteWith);
    auto targetPreset = m_presetManager.findPreset(presetToOverwrite);

    PresetManagerUseCases useCases(&m_presetManager);

    if(targetPreset)
    {
      if(sourcePreset)
      {
        useCases.overwritePresetWithPreset(targetPreset, sourcePreset);
      }
      else
      {
        useCases.overwritePreset(targetPreset);
      }
    }
    else
    {
      useCases.createBankAndStoreEditBuffer();
    }
  });

  addAction("copy-preset-above", [&](std::shared_ptr<NetworkRequest> request) {
    auto presetToCopy = request->get("presetToCopy");
    auto presetAnchor = request->get("anchor");
    PresetManagerUseCases useCases(&m_presetManager);
    useCases.copyPresetAbove(presetToCopy, presetAnchor);
  });

  addAction("copy-preset-below", [&](std::shared_ptr<NetworkRequest> request) {
    auto presetToCopy = request->get("presetToCopy");
    auto presetAnchor = request->get("anchor");
    PresetManagerUseCases useCases(&m_presetManager);
    useCases.copyPresetBelow(presetToCopy, presetAnchor);
  });

  addAction("insert-editbuffer-above", [&](std::shared_ptr<NetworkRequest> request) {
    auto presetAnchor = request->get("anchor");
    auto uuid = request->get("uuid");
    PresetManagerUseCases useCases(&m_presetManager);
    useCases.insertEditBufferAbove(presetAnchor, uuid);
  });

  addAction("insert-editbuffer-below", [&](std::shared_ptr<NetworkRequest> request) {
    auto presetAnchor = request->get("anchor");
    auto uuid = request->get("uuid");
    PresetManagerUseCases useCases(&m_presetManager);
    useCases.insertEditBufferBelow(presetAnchor, uuid);
  });

  addAction("overwrite-preset-with-editbuffer", [&](std::shared_ptr<NetworkRequest> request) {
    auto presetToOverwrite = request->get("presetToOverwrite");
    PresetManagerUseCases useCase(&m_presetManager);
    useCase.overwritePreset(Uuid { presetToOverwrite });
  });

  addAction("append-preset", [&](std::shared_ptr<NetworkRequest> request) mutable {
    auto b = m_presetManager.getSelectedBank();
    std::string fallBack = b ? b->getUuid().raw() : "";
    auto bankToAppendTo = request->get("bank-uuid", fallBack);
    auto uuid = request->get("uuid");

    if(auto bank = m_presetManager.findBank(bankToAppendTo))
    {
      PresetManagerUseCases useCase(&m_presetManager);
      useCase.appendPresetWithUUID(bank, uuid);
    }
  });

  addAction("append-preset-to-bank", [&](std::shared_ptr<NetworkRequest> request) mutable {
    auto bankUuid = request->get("bank-uuid");
    auto presetUuid = request->get("preset-uuid");

    if(auto bank = m_presetManager.findBank(bankUuid))
    {
      PresetManagerUseCases useCases(&m_presetManager);
      useCases.appendPreset(bank);
    }
  });

  addAction("set-order-number", [&](std::shared_ptr<NetworkRequest> request) mutable {
    auto uuid = request->get("uuid");
    auto orderNumber = std::max(1ULL, std::stoull(request->get("order-number"))) - 1;

    if(auto bank = m_presetManager.findBank(uuid))
    {
      PresetManagerUseCases useCase(&m_presetManager);
      useCase.setOrderNumber(bank, orderNumber);
    }
  });

  addAction("insert-preset", [&](std::shared_ptr<NetworkRequest> request) mutable {
    auto selUuid = request->get("seluuid");
    PresetManagerUseCases useCases(&m_presetManager);

    if(auto bank = m_presetManager.findBankWithPreset(selUuid))
    {
      auto desiredPresetPos = bank->getPresetPosition(selUuid) + 1;
      useCases.insertPreset(bank, desiredPresetPos);
    }
  });

  addAction("select-preset", [&](std::shared_ptr<NetworkRequest> request) mutable {
    Glib::ustring presetUUID = request->get("uuid");
    PresetManagerUseCases useCase(&m_presetManager);
    useCase.selectPreset(presetUUID);
  });

  addAction("delete-preset", [&](std::shared_ptr<NetworkRequest> request) mutable {
    auto presetUUID = request->get("uuid");
    auto deleteBank = request->get("delete-bank") == "true";

    if(auto srcBank = m_presetManager.findBankWithPreset(presetUUID))
    {
      if(deleteBank)
      {
        PresetManagerUseCases useCase(&m_presetManager);
        useCase.deleteBank(srcBank);
      }
      else
      {
        BankUseCases useCae(srcBank);
        useCae.deletePreset(presetUUID);
      }
    }
  });

  addAction("delete-presets", [&](std::shared_ptr<NetworkRequest> request) mutable {
    std::vector<std::string> strs;
    auto csv = request->get("presets");
    auto deleteBanks = request->get("delete-bank") == "true";
    boost::split(strs, csv, boost::is_any_of(","));

    PresetManagerUseCases useCase(&m_presetManager);
    useCase.deletePresets(strs, deleteBanks);
  });

  addAction("load-preset", [&](std::shared_ptr<NetworkRequest> request) mutable {
    auto uuid = request->get("uuid");
    EditBufferUseCases useCase(m_presetManager.getEditBuffer());
    useCase.undoableLoad(uuid);
  });

  addAction("set-position", [&](std::shared_ptr<NetworkRequest> request) mutable {
    auto uuid = request->get("uuid");
    auto x = request->get("x");
    auto y = request->get("y");
    auto undock = request->get("undock");

    if(auto bank = m_presetManager.findBank(uuid))
    {
      BankUseCases useCase(bank);
      useCase.moveBank(x, y, undock == "1");
    }
  });

  addAction("create-new-bank-from-preset", [&](std::shared_ptr<NetworkRequest> request) mutable {
    auto uuid = request->get("preset");
    auto x = request->get("x");
    auto y = request->get("y");

    PresetManagerUseCases useCase(&m_presetManager);
    useCase.createBankFromPreset(uuid, x, y);
  });

  addAction("create-new-bank-from-presets", [&](std::shared_ptr<NetworkRequest> request) mutable {
    auto csv = request->get("presets");
    auto x = request->get("x");
    auto y = request->get("y");

    PresetManagerUseCases useCase(&m_presetManager);
    useCase.createBankFromPresets(csv, x, y);
  });

  addAction("drop-bank-on-bank", [&](std::shared_ptr<NetworkRequest> request) mutable {
    Glib::ustring receiverUuid = request->get("receiver");
    Glib::ustring bankUuid = request->get("bank");

    if(auto tgtBank = m_presetManager.findBank(receiverUuid))
    {
      if(auto srcBank = m_presetManager.findBank(bankUuid))
      {
        BankUseCases useCase(tgtBank);
        useCase.dropBank(srcBank);
      }
    }
  });

  addAction("drop-presets-on-bank", [&](std::shared_ptr<NetworkRequest> request) mutable {
    Glib::ustring bankUUID = request->get("bank");
    Glib::ustring csv = request->get("presets");

    if(auto tgtBank = m_presetManager.findBank(bankUUID))
    {
      BankUseCases useCase(tgtBank);
      useCase.dropPresets(csv);
    }
  });

  addAction("insert-bank-above", [&](std::shared_ptr<NetworkRequest> request) mutable { insertBank(request, 0); });

  addAction("insert-bank-below", [&](std::shared_ptr<NetworkRequest> request) mutable { insertBank(request, 1); });

  addAction("overwrite-preset-with-bank", [&](std::shared_ptr<NetworkRequest> request) mutable {
    auto anchorUuid = request->get("anchor");
    auto bankUuid = request->get("bank");
    if(auto srcbank = m_presetManager.findBank(bankUuid))
    {
      if(auto tgtBank = m_presetManager.findBankWithPreset(anchorUuid))
      {
        BankUseCases useCase(tgtBank);
        useCase.dropBankOnPreset(srcbank, anchorUuid);
      }
    }
  });

  addAction("import-bank", [&](std::shared_ptr<NetworkRequest> request) mutable {
    auto xml = request->get("xml");
    auto x = request->get("x");
    auto y = request->get("y");
    auto fileName = request->get("fileName");
    MemoryInStream stream(xml, false);
    importBank(stream, x, y, fileName);
  });

  addAction("set-preset-attribute", [&](std::shared_ptr<NetworkRequest> request) mutable {
    auto csv = request->get("csv");
    auto value = request->get("value");

    std::vector<std::string> strs;
    boost::split(strs, csv, boost::is_any_of(","));

    PresetUseCases useCase(preset);
    useCase.setAttribute(strs, value);
  });

  addAction("set-bank-attribute", [&](std::shared_ptr<NetworkRequest> request) mutable {
    auto bankUUID = request->get("uuid");
    auto key = request->get("key");
    auto value = request->get("value");

    if(auto bank = m_presetManager.findBank(bankUUID))
    {
      BankUseCases useCase(bank);
      useCase.setAttribute(key, value);
    }
  });

  addAction("move", [&](std::shared_ptr<NetworkRequest> request) mutable {
    auto bankUUID = request->get("bank");
    auto value = request->get("direction");

    PresetManagerUseCases useCase(&m_presetManager);

    if(auto bank = m_presetManager.getSelectedBank())
    {
      if(value == "LeftByOne")
        useCase.moveLeft(bank);
      else if(value == "RightByOne")
        useCase.moveRight(bank);
    }
  });

  addAction("insert-bank-in-cluster", [&](std::shared_ptr<NetworkRequest> request) mutable {
    const auto insertedUUID = request->get("bank-to-insert");
    const auto insertedAtUUID = request->get("bank-inserted-at");
    const auto orientation = request->get("orientation");

    PresetManagerUseCases useCase(&m_presetManager);

    if(auto bankToInsert = m_presetManager.findBank(insertedUUID))
    {
      if(auto bankAtInsertPosition = m_presetManager.findBank(insertedAtUUID))
      {
        useCase.insertBankInCluster(bankToInsert, bankAtInsertPosition, orientation);
      }
    }
  });

  addAction("dock-banks", [&](std::shared_ptr<NetworkRequest> request) mutable {
    const auto droppedOntoBankUuid = request->get("droppedOntoBank");
    const auto draggedBankUuid = request->get("draggedBank");
    const auto droppedAt = request->get("droppedAt");
    const auto x = request->get("x");
    const auto y = request->get("y");

    if(auto droppedOntoBank = m_presetManager.findBank(droppedOntoBankUuid))
    {
      if(auto draggedBank = m_presetManager.findBank(draggedBankUuid))
      {
        auto scope = presetManager.getUndoScope().startTransaction(
            "Dock Banks '%0' and '%1'", droppedOntoBank->getName(true), draggedBank->getName(true));
        auto transaction = scope->getTransaction();

        if(droppedAt == "North")
        {
          droppedOntoBank->attachBank(transaction, draggedBank->getUuid(), Bank::AttachmentDirection::top);
        }
        else if(droppedAt == "West")
        {
          droppedOntoBank->attachBank(transaction, draggedBank->getUuid(), Bank::AttachmentDirection::left);
        }
        else if(droppedAt == "South")
        {
          draggedBank->attachBank(transaction, droppedOntoBank->getUuid(), Bank::AttachmentDirection::top);
        }
        else if(droppedAt == "East")
        {
          draggedBank->attachBank(transaction, droppedOntoBank->getUuid(), Bank::AttachmentDirection::left);
        }

        draggedBank->setX(transaction, x);
        draggedBank->setY(transaction, y);

        m_presetManager.sanitizeBankClusterRelations(transaction);
      }
    }
  });

  addAction("undock-bank", [&](std::shared_ptr<NetworkRequest> request) mutable {
    const auto uuid = request->get("uuid");
    const auto x = request->get("x");
    const auto y = request->get("y");

    if(auto bank = m_presetManager.findBank(uuid))
    {
      if(auto attached = m_presetManager.findBank(bank->getAttachedToBankUuid()))
      {
        auto parentBankName = attached->getName(true);
        auto scope = presetManager.getUndoScope().startTransaction("Detached Bank '%0' from '%1'", bank->getName(true),
                                                                   parentBankName);
        auto transaction = scope->getTransaction();

        bank->attachBank(transaction, Uuid::none(), Bank::AttachmentDirection::none);
        bank->setX(transaction, x);
        bank->setY(transaction, y);
      }
    }
  });

  addAction("move-all-banks", [&](std::shared_ptr<NetworkRequest> request) mutable {
    auto x = std::stof(request->get("x"));
    auto y = std::stof(request->get("y"));
    PresetManagerUseCases useCases(&m_presetManager);
    useCases.moveAllBanks(x, y);
  });

  addAction("sort-bank-numbers", [&](auto request) mutable {
    PresetManagerUseCases pmUseCases(&presetManager);
    pmUseCases.sortBankNumbers();
  });
}

BankActions::~BankActions()
{
}

void BankActions::insertBank(std::shared_ptr<NetworkRequest> request, size_t offset)
{
  Glib::ustring anchorUuid = request->get("anchor");
  Glib::ustring bankUuid = request->get("bank");

  if(auto preset = m_presetManager.findPreset(anchorUuid))
    if(auto targetBank = static_cast<Bank *>(preset->getParent()))
      if(auto bank = m_presetManager.findBank(bankUuid))
        if(bank != targetBank)
        {
          auto anchorPos = targetBank->getPresetPosition(anchorUuid);
          insertBank(bank, targetBank, anchorPos + offset);
        }
}

void BankActions::insertBank(Bank *bank, Bank *targetBank, size_t insertPos)
{
  auto scope = m_presetManager.getUndoScope().startTransaction("Drop bank '%0' into bank '%1'", bank->getName(true),
                                                               targetBank->getName(true));
  auto transaction = scope->getTransaction();
  size_t i = 0;

  bank->forEachPreset([&](auto p) {
    targetBank->insertPreset(transaction, insertPos + i, std::make_unique<Preset>(targetBank, *p, true));
    i++;
  });
}

bool BankActions::handleRequest(const Glib::ustring &path, std::shared_ptr<NetworkRequest> request)
{
  if(super::handleRequest(path, request))
    return true;

  if(path.find("/presets/banks/download-bank/") == 0)
  {
    PerformanceTimer timer(__PRETTY_FUNCTION__);

    if(auto httpRequest = std::dynamic_pointer_cast<HTTPRequest>(request))
    {
      Glib::ustring uuid = request->get("uuid");

      if(auto bank = m_presetManager.findBank(uuid))
      {
        httpRequest->setHeader("Content-Disposition", "attachment; filename=\"" + bank->getName(true) + ".xml\"");
        XmlWriter writer(request->createStream("text/xml", false));
        PresetBankSerializer serializer(bank);
        serializer.write(writer, VersionAttribute::get());

        auto scope = UNDO::Scope::startTrashTransaction();
        auto transaction = scope->getTransaction();
        bank->setAttribute(transaction, "Name of Export File", "(via Browser)");
        bank->setAttribute(transaction, "Date of Export File", TimeTools::getAdjustedIso());
      }

      return true;
    }
  }

  if(path.find("/presets/banks/download-preset/") == 0)
  {
    PerformanceTimer timer(__PRETTY_FUNCTION__);

    if(auto httpRequest = std::dynamic_pointer_cast<HTTPRequest>(request))
    {
      XmlWriter writer(request->createStream("text/xml", false));

      Glib::ustring uuid = request->get("uuid");

      Preset *preset = nullptr;
      auto ebAsPreset = std::make_unique<Preset>(&m_presetManager, *m_presetManager.getEditBuffer());

      if(uuid.empty())
      {
        preset = ebAsPreset.get();
      }
      else if(auto p = m_presetManager.findPreset(uuid))
      {
        preset = p;
      }
      else
      {
        DebugLevel::warning("Could not download Preset!");
        return false;
      }

      PresetSerializer serializer(preset);
      httpRequest->setHeader("Content-Disposition", "attachment; filename=\"" + preset->getName() + ".xml\"");
      serializer.write(writer, VersionAttribute::get());

      return true;
    }
  }

  return false;
}

Bank *BankActions::importBank(InStream &stream, Glib::ustring x, Glib::ustring y, const Glib::ustring &fileName)
{
  UNDO::Scope::tTransactionScopePtr scope = m_presetManager.getUndoScope().startTransaction("Import new Bank");
  auto transaction = scope->getTransaction();
  return importBank(transaction, stream, x, y, fileName);
}

Bank *BankActions::importBank(UNDO::Transaction *transaction, InStream &stream, Glib::ustring x, Glib::ustring y,
                              const Glib::ustring &fileName)
{
  auto autoLoadOff = Application::get().getSettings()->getSetting<DirectLoadSetting>()->scopedOverlay(
      BooleanSettings::BOOLEAN_SETTING_FALSE);

  auto newBank = m_presetManager.addBank(transaction, std::make_unique<Bank>(&m_presetManager));

  XmlReader reader(stream, transaction);
  reader.read<PresetBankSerializer>(newBank, true);

  newBank->setAttachedToBank(transaction, Uuid::none());
  newBank->setAttachedDirection(transaction, to_string(Bank::AttachmentDirection::none));

  if(x.empty() || y.empty())
  {
    auto pos = m_presetManager.calcDefaultBankPositionFor(newBank);
    x = to_string(pos.first);
    y = to_string(pos.second);
  }

  newBank->setX(transaction, x);
  newBank->setY(transaction, y);

  newBank->ensurePresetSelection(transaction);
  newBank->setAttribute(transaction, "Name of Import File", fileName);
  newBank->setAttribute(transaction, "Date of Import File", TimeTools::getAdjustedIso());
  newBank->setAttribute(transaction, "Name of Export File", "");
  newBank->setAttribute(transaction, "Date of Export File", "");

  m_presetManager.ensureBankSelection(transaction);

  Application::get().getHWUI()->setFocusAndMode(FocusAndMode(UIFocus::Presets, UIMode::Select));

  return newBank;
}

void BankActions::insertBankInCluster(Bank *bankToInsert, Bank *bankAtInsert,
                                      const Glib::ustring directionSeenFromBankInCluster)
{
}

#pragma once

#include "http/RPCActionManager.h"
#include "libundo/undo/Scope.h"
#include <xml/FileInStream.h>

class PresetManager;
class Preset;
class Bank;

class BankActions : public RPCActionManager
{
  typedef RPCActionManager super;

 public:
  explicit BankActions(PresetManager &presetManager);
  ~BankActions() override;

  bool handleRequest(const Glib::ustring &path, std::shared_ptr<NetworkRequest> request) override;

  Bank *importBank(InStream &stream, Glib::ustring x = "", Glib::ustring y = "", const Glib::ustring &fileName = "");
  Bank *importBank(UNDO::Transaction *transaction, InStream &stream, Glib::ustring x = "", Glib::ustring y = "",
                   const Glib::ustring &fileName = "");

 private:
  typedef Preset *tPresetPtr;

  void insertBank(Bank *bank, Bank *targetBank, size_t insertPos);
  void insertBank(std::shared_ptr<NetworkRequest> request, size_t offset);

  PresetManager &m_presetManager;

  void insertBankInCluster(Bank *bankToInsert, Bank *bankAtInsert, Glib::ustring directionSeenFromBankInCluster);
};

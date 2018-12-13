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
  BankActions(PresetManager &presetManager);
  virtual ~BankActions();

  bool handleRequest(const Glib::ustring &path, shared_ptr<NetworkRequest> request) override;

  Bank *importBank(InStream &stream, Glib::ustring x = "", Glib::ustring y = "", const Glib::ustring &fileName = "");

 private:
  typedef Preset *tPresetPtr;

  void dropPresets(UNDO::Transaction *transaction, const Glib::ustring &anchorUUID, int offset,
                   const Glib::ustring &csv);
  bool loadPresetAtRelativePosition(int offset);
  void insertBank(Bank *bank, Bank *targetBank, size_t insertPos);
  void insertBank(shared_ptr<NetworkRequest> request, size_t offset);
  Glib::ustring guessNameBasedOnEditBuffer() const;

  PresetManager &m_presetManager;

  void insertBankInCluster(Bank *bankToInsert, Bank *bankAtInsert, const ustring directionSeenFromBankInCluster);
};

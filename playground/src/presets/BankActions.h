#pragma once

#include "http/RPCActionManager.h"
#include "libundo/undo/Scope.h"
#include <xml/FileInStream.h>

class PresetManager;
class PresetBank;
class Preset;

class BankActions : public RPCActionManager
{
  typedef RPCActionManager super;
  typedef shared_ptr<PresetBank> tBankPtr;

 public:
  BankActions(PresetManager &presetManager);
  virtual ~BankActions();

  bool handleRequest(const Glib::ustring &path, shared_ptr<NetworkRequest> request) override;

  tBankPtr importBank(InStream &stream, const Glib::ustring &x = "", const Glib::ustring &y = "",
                      const Glib::ustring &fileName = "");

 private:
  typedef shared_ptr<Preset> tPresetPtr;

  void dropPresets(UNDO::TransactionCreationScope::tTransactionPtr transaction, const Glib::ustring &anchorUUID,
                   int offset, const Glib::ustring &csv);
  bool loadPresetAtRelativePosition(int offset);
  void insertBank(tBankPtr bank, tBankPtr targetBank, size_t insertPos);
  void insertBank(shared_ptr<NetworkRequest> request, size_t offset);
  Glib::ustring guessNameBasedOnEditBuffer() const;

  PresetManager &m_presetManager;
};

#pragma once

#include "http/RPCActionManager.h"
#include <libsoup/soup-message-body.h>
#include <libundo/undo/Scope.h>
#include <libundo/undo/Transaction.h>

class PresetManager;
class PresetBank;
class Preset;
class HTTPRequest;

class PresetManagerActions : public RPCActionManager
{
 private:
  typedef RPCActionManager super;
  PresetManager& m_presetManager;

 public:
  PresetManagerActions(PresetManager& presetManager);
  virtual ~PresetManagerActions();

  bool handleRequest(const Glib::ustring& path, shared_ptr<NetworkRequest> request) override;

 private:
  void handleImportBackupFile(UNDO::TransactionCreationScope::tTransactionPtr transaction, SoupBuffer* buffer,
                              shared_ptr<HTTPRequest> http);

  typedef shared_ptr<PresetBank> tBankPtr;
  typedef shared_ptr<Preset> tPresetPtr;
};

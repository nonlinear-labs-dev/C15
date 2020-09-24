#pragma once

#include "http/RPCActionManager.h"
#include <libsoup/soup-message-body.h>

namespace UNDO
{
  class Transaction;
}

class PresetManager;
class Preset;
class HTTPRequest;

class PresetManagerActions : public RPCActionManager
{
 private:
  typedef RPCActionManager super;
  PresetManager& m_presetManager;

 public:
  PresetManagerActions(PresetManager& presetManager);

  bool handleRequest(const Glib::ustring& path, std::shared_ptr<NetworkRequest> request) override;

 private:
  void handleImportBackupFile(UNDO::Transaction* transaction, SoupBuffer* buffer, std::shared_ptr<HTTPRequest> http);

  typedef Preset* tPresetPtr;
};

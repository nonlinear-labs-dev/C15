#pragma once

#include <libundo/undo/Transaction.h>
class PresetManager;
class Preset;
class Bank;

class PresetManagerUseCases
{
 public:
  explicit PresetManagerUseCases(PresetManager* pm);

  //Store Actions
  void overwritePreset(Preset* preset);
  void overwritePresetWithPreset(Preset* target, Preset* source);
  void insertPresetWithUUID(Bank* bank, size_t pos, const std::string& uuid);
  void insertPreset(Bank* bank, size_t pos);
  void appendPreset(Bank* bank);
  void appendPresetWithUUID(Bank* bank, const std::string& uuid);
  void createBankAndStoreEditBuffer();

 private:
  PresetManager* m_presetManager;
  void onStore(UNDO::Transaction* transaction, Preset* preset);
  void updateSyncSettingOnPresetStore(UNDO::Transaction* transaction) const;
};

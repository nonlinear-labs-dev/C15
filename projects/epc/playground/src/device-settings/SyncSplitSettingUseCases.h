#pragma once
#include <libundo/undo/Transaction.h>

class SplitPointSyncParameters;

class SyncSplitSettingUseCases
{
 public:
  static SyncSplitSettingUseCases& get();

  void updateFromWebUI(const Glib::ustring& value);
  void enableSyncSetting();
  void disableSyncSetting();
  void enableSyncSetting(UNDO::Transaction*);
  void disableSyncSetting(UNDO::Transaction*);

  void toggleSyncSetting();

 private:
  void undoableSetSetting(UNDO::Transaction* transaction, bool state);
  static SplitPointSyncParameters* getSyncSetting();

  SyncSplitSettingUseCases() = default;
};

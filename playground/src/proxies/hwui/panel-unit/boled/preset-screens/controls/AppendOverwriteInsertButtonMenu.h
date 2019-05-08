#pragma once

#include "proxies/hwui/controls/ButtonMenu.h"
#include "device-settings/PresetStoreModeSetting.h"
#include <libundo/undo/Scope.h>

class PresetManagerLayout;
class PresetBank;
class Preset;

class AppendOverwriteInsertButtonMenu : public ButtonMenu
{
 private:
  typedef ButtonMenu super;

 public:
  AppendOverwriteInsertButtonMenu(PresetManagerLayout &parent, const Rect &rect);
  virtual ~AppendOverwriteInsertButtonMenu();

  void selectButton(size_t i) override;

 private:
  typedef UNDO::Scope::tTransactionPtr tTransactionPtr;

  PresetStoreModeSettings indexToEnum(size_t i) const;
  size_t enumToIndex(PresetStoreModeSettings i) const;

  shared_ptr<Preset> overwritePreset(shared_ptr<Preset> preset);
  shared_ptr<Preset> overwritePreset(tTransactionPtr scope, shared_ptr<Preset> preset);
  void insertPreset(shared_ptr<PresetBank> bank, size_t pos, bool modified);
  void appendPreset(shared_ptr<PresetBank> bank, bool modified);
  void pushRenameScreen();
  bool animate();

  void append(shared_ptr<PresetBank> bank, bool modified);
  void insert(shared_ptr<PresetBank> bank, shared_ptr<Preset> tgtPreset, bool modified);
  void overwrite(shared_ptr<PresetBank> bank, shared_ptr<Preset> tgtPreset, bool modified);

  void executeAction();

  PresetManagerLayout &m_parent;
};

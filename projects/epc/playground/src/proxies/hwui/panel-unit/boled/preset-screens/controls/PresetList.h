#pragma once

#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetListBase.h>
#include <tools/Uuid.h>
#include <sigc++/connection.h>
#include <libundo/undo/TransactionCreationScope.h>
#include <libundo/undo/Scope.h>

class Preset;
class PresetManager;
class Bank;

class PresetList : public PresetListBase
{
  using super = PresetListBase;

 public:
  PresetList(const Rect& pos, bool showBankArrows);
  ~PresetList() override;

  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
  void onRotary(int inc, ButtonModifiers modifiers) override;

  std::pair<size_t, size_t> getSelectedPosition() const override;

 private:
  void onBankSelectionChanged(const Uuid& selectedBank);
  void onBankChanged();
  void onEditBufferChanged();
  void onMidiBankChanged(const Uuid& midiBank);

  sigc::connection m_bankChangedConnection;
  Uuid m_uuidOfLastLoadedPreset;

 protected:
  virtual void stepPresetSelection(int inc, PresetManager* pm, Bank* bank) const;
  void stepBankSelection(int inc, const ButtonModifiers& modifiers, PresetManager* pm) const;
};

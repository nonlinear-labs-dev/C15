#pragma once

#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetListBase.h>
#include <tools/Uuid.h>

class PresetList : public PresetListBase
{
  using super = PresetListBase;

 public:
  PresetList(const Rect& pos, bool showBankArrows);
  ~PresetList() override;

  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
  void onRotary(int inc, ButtonModifiers modifiers) override;

  std::pair<size_t, size_t> getSelectedPosition() const override;
  Preset* getPresetAtSelected();

 private:
  void onBankSelectionChanged(const Uuid& selectedBank);
  void onBankChanged();
  void onEditBufferChanged();

  sigc::connection m_bankChangedConnection;
  Uuid m_uuidOfLastLoadedPreset;
};

// Make this the new class:
//Replace Whole PresetListBase with generic traversal of Presets instead of strong coupling with selection

class GenericPresetList : public PresetList
{
 public:
  explicit GenericPresetList(const Point& p);

  void incBankSelection();
  void decBankSelection();
  void incPresetSelection();
  void decPresetSelection();

  virtual void action() = 0;

 protected:
  void movePresetSelection(int inc);
  void moveBankSelection(int inc);
};

class PresetListVGSelect : public GenericPresetList
{
 public:
  explicit PresetListVGSelect(const Point& p);
  void action() override;
 protected:
  PresetManager::AutoLoadBlocker blocker;
};
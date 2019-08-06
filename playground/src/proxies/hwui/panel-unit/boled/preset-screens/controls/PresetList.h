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

 private:
  void onBankSelectionChanged(const Uuid& selectedBank);
  void onBankChanged();
  void onEditBufferChanged();

  sigc::connection m_bankChangedConnection;
  Uuid m_uuidOfLastLoadedPreset;
};

class GenericPresetList : public PresetList
{
 public:
  explicit GenericPresetList(const Point& p);
};

class PresetListVGSelect : public GenericPresetList
{
 public:
  explicit PresetListVGSelect(const Point& p);
};
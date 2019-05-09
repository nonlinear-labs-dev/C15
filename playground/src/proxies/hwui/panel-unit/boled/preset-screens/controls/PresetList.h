#pragma once

#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetListBase.h>

class PresetList : public PresetListBase
{
  using super = PresetListBase;

 public:
  PresetList(const Rect &pos, bool showBankArrows);
  virtual ~PresetList();

  virtual bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
  virtual void onRotary(int inc, ButtonModifiers modifiers) override;

  std::pair<size_t, size_t> getSelectedPosition() const override;

 private:
  void onBankSelectionChanged();
  void onBankChanged();

  sigc::connection m_bankChangedConnection;
};

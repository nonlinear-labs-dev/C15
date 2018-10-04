#pragma once

#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetListBase.h>

class PresetList : public PresetListBase
{
    using super = PresetListBase;

  public:
    PresetList(const Rect &pos, bool showBankArrows);
    virtual ~PresetList();

    virtual bool onButton(int i, bool down, ButtonModifiers modifiers) override;
    virtual void onRotary(int inc, ButtonModifiers modifiers) override;

    std::pair<int, int> getSelectedPosition() const override;

  private:
    void onBankSelectionChanged(shared_ptr<PresetBank> bank);
    void onBankChanged();

    sigc::connection m_bankChangedConnection;
    void onRestore();
};


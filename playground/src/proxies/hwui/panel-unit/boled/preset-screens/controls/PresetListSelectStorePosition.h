#pragma once

#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetListBase.h>
#include <memory>

class PresetBank;
class Preset;

class PresetListSelectStorePosition : public PresetListBase
{
    using super = PresetListBase;

  public:
    PresetListSelectStorePosition(const Rect &pos, bool showBankArrows);
    virtual ~PresetListSelectStorePosition();

    virtual bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
    virtual void onRotary(int inc, ButtonModifiers modifiers) override;
    std::pair<int, int> getSelectedPosition() const override;
    void movePresetSelection();
    void initBankAndPreset();
    void sanitizeBankPosition(std::shared_ptr<PresetManager> pm);
    void sanitizePresetPosition(std::shared_ptr<PresetBank> bank);

  private:
    void movePresetSelection(int moveBy);
    void moveBankSelection(int moveBy);
    void onChange();
    void onBankChanged();

    static constexpr int invalidIndex = -1;

    int m_presetPosition = invalidIndex;
    int m_bankPosition = invalidIndex;
    Glib::ustring m_selectedPreset;
    sigc::connection m_bankConnection;
};


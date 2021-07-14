#pragma once

#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetListBase.h>
#include <memory>
#include <tools/Uuid.h>

class PresetManager;
class Bank;
class Preset;
class StoreModeData;

class PresetListSelectStorePosition : public PresetListBase
{
  using super = PresetListBase;

 public:
  PresetListSelectStorePosition(const Rect &pos, bool showBankArrows, StoreModeData *pod);
  ~PresetListSelectStorePosition() override;

  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
  void onRotary(int inc, ButtonModifiers modifiers) override;
  std::pair<size_t, size_t> getSelectedPosition() const override;
  void initBankAndPreset();
  void sanitizeBankPosition(PresetManager *pm);
  void sanitizePresetPosition(Bank *bank);

 private:
  void movePresetSelection(int moveBy);
  void moveBankSelection(int moveBy);
  void onChange();
  void onBankChanged();

  static constexpr size_t invalidIndex = size_t(-1);

  Preset *m_selectedPreset = nullptr;
  sigc::connection m_bankConnection;
  StoreModeData *m_storeModeData;
};

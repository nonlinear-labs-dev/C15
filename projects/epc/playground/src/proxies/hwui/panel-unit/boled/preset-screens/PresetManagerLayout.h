#pragma once

#include <proxies/hwui/Layout.h>
#include <proxies/hwui/HWUIEnums.h>
#include <functional>
#include <presets/StoreModeData.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/LoadModeMenu.h>
#include <presets/PresetPartSelection.h>

class BankAndPresetNumberLabel;

class NumPresetsInBankLabel;

class PresetManager;
class PresetListBase;
class Setting;
class DirectLoadSetting;
class ButtonMenu;
class Button;
class BankButton;

class PresetManagerLayout : public Layout
{
 private:
  typedef Layout super;

 public:
  PresetManagerLayout(FocusAndMode focusAndMode, FocusAndMode oldFocusAndMode);
  ~PresetManagerLayout() override;

  void setFocusAndMode(FocusAndMode focusAndMode);

  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
  bool onRotary(int inc, ButtonModifiers modifiers) override;

  bool animateSelectedPreset(std::function<void()> cb);
  void animateSelectedPresetIfInLoadPartMode(std::function<void()> cb);

  std::pair<size_t, size_t> getSelectedPosition() const;
  std::unique_ptr<StoreModeData> &getStoreModePtr();

 private:
  void setup();
  PresetManager *getPresetManager() const;

  void setupBankFocus();
  void setupPresetFocus();
  void setupPresetEdit();
  void setupPresetSelect();
  void setupPresetStore();
  void setupBankEdit();
  void setupBankSelect();
  void setupBankStore();

  LoadModeMenu *m_loadMode = nullptr;
  BankButton *m_bankButton = nullptr;
  ButtonMenu *m_menu = nullptr;
  PresetListBase *m_presets = nullptr;
  FocusAndMode m_focusAndMode;
  FocusAndMode m_oldFocusAndMode;
  StoreModeData *getStoreModeData();

  sigc::connection m_dlSettingConnection;
  sigc::connection m_loadToPartConnection;

  void setStoreModeData(std::unique_ptr<StoreModeData> ptr);
};

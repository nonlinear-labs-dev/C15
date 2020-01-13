#pragma once

#include <proxies/hwui/DFBLayout.h>
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/InvertedLabel.h>
#include <functional>
#include <presets/StoreModeData.h>

class BankAndPresetNumberLabel;

class NumPresetsInBankLabel;

class PresetManager;
class PresetListBase;
class Setting;
class LoadModeSetting;
class ButtonMenu;
class Button;

class PresetManagerLayout : public DFBLayout
{
 private:
  typedef DFBLayout super;

 public:
  PresetManagerLayout(FocusAndMode focusAndMode);
  virtual ~PresetManagerLayout();

  void setFocusAndMode(FocusAndMode focusAndMode);

  virtual bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
  virtual bool onRotary(int inc, ButtonModifiers modifiers) override;

  bool animateSelectedPreset(std::function<void()> cb);
  void animateSelectedPresetIfInLoadPartMode(std::function<void()> cb);

  std::pair<size_t, size_t> getSelectedPosition() const;
  std::unique_ptr<StoreModeData> &getStoreModePtr();

 private:
  void updateAutoLoadButton(const Setting *setting);
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

  ButtonMenu *m_menu = nullptr;
  Button *m_autoLoad = nullptr;
  PresetListBase *m_presets = nullptr;
  FocusAndMode m_focusAndMode;
  StoreModeData *getStoreModeData();
  void setStoreModeData(std::unique_ptr<StoreModeData> ptr);

  void loadSelectedPresetAccordingToLoadType();

  void openPartChooser();
};

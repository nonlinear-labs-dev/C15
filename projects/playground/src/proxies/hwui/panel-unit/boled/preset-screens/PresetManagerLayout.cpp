#include <utility>

#include <Application.h>
#include <device-settings/DirectLoadSetting.h>
#include <device-settings/Settings.h>
#include <presets/EditBuffer.h>
#include <presets/Bank.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/buttons.h>
#include <proxies/hwui/controls/Button.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/AppendOverwriteInsertButtonMenu.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/BankAndPresetNumberLabel.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/BankEditButtonMenu.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/InvertedLabel.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/NumBanksLabel.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/NumPresetsInBankLabel.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetEditButtonMenu.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetList.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetListSelectStorePosition.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/PresetManagerLayout.h>
#include <proxies/hwui/panel-unit/EditPanel.h>
#include <functional>
#include <memory>
#include <vector>
#include <proxies/hwui/panel-unit/boled/undo/UndoIndicator.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/AnyParameterLockedIndicator.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/LoadModeMenu.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/VoiceGroupIndicator.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MuteIndicator.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/BankButton.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/LoadToPartPresetList.h>
#include <proxies/hwui/HWUIHelper.h>
#include "presets/Preset.h"
#include "SelectVoiceGroupLayout.h"

PresetManagerLayout::PresetManagerLayout(FocusAndMode focusAndMode, FocusAndMode oldFocusAndMode)
    : super(Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled())
    , m_focusAndMode(focusAndMode)
    , m_oldFocusAndMode(oldFocusAndMode)
{
  setup();

  m_dlSettingConnection = Application::get().getSettings()->getSetting<DirectLoadSetting>()->onChange(
      sigc::hide(sigc::mem_fun(this, &PresetManagerLayout::setup)));
}

PresetManagerLayout::~PresetManagerLayout()
{
  m_focusAndMode.mode = UIMode::Edit;
  m_dlSettingConnection.disconnect();
}

void PresetManagerLayout::setFocusAndMode(FocusAndMode focusAndMode)
{
  if(m_focusAndMode != focusAndMode)
  {
    m_focusAndMode = focusAndMode;
    setup();
  }
}

void PresetManagerLayout::setup()
{
  m_menu = nullptr;
  m_presets = nullptr;
  m_loadMode = nullptr;
  m_bankButton = nullptr;

  clear();

  if(m_focusAndMode.focus == UIFocus::Banks)
  {
    setupBankFocus();
  }
  else
  {
    setupPresetFocus();
  }
}

PresetManager *PresetManagerLayout::getPresetManager() const
{
  return Application::get().getPresetManager();
}

void PresetManagerLayout::setupBankFocus()
{
  m_bankButton = addControl(new BankButton({ 3, 36, 58, 57 }, true));

  switch(m_focusAndMode.mode)
  {
    case UIMode::Edit:
      setupBankEdit();
      break;

    case UIMode::Select:
      setupBankSelect();
      break;

    case UIMode::Store:
      setupBankStore();
      break;
  }
}

void PresetManagerLayout::setupBankEdit()
{
  if(getStoreModeData() != nullptr)
  {
    setStoreModeData(nullptr);
  }
  addControl(new BankAndPresetNumberLabel(Rect(0, 0, 64, 14)));
  addControl(new InvertedLabel("Edit", Rect(8, 26, 48, 12)))->setHighlight(true);

  addControl(new UndoIndicator(Rect { 5, 14, 15, 5 }));

  addControl(new AnyParameterLockedIndicator(Rect(244, 2, 10, 11)));

  m_menu = addControl(new BankEditButtonMenu(Rect(195, 1, 58, 62)));
  m_presets = addControl(new PresetList(Rect(64, 0, 128, 63), true));
  m_presets->setBankFocus();
}

void PresetManagerLayout::setupBankSelect()
{
  if(getStoreModeData() != nullptr)
  {
    setStoreModeData(nullptr);
  }
  addControl(new BankAndPresetNumberLabel(Rect(0, 0, 64, 14)));
  addControl(new NumBanksLabel(Rect(208, 1, 32, 14)))->setHighlight(false);

  addControl(new VoiceGroupIndicator(Rect(2, 15, 16, 16)));
  addControl(new UndoIndicator(Rect(27, 16, 10, 8)));

  addControl(new AnyParameterLockedIndicator(Rect(244, 2, 10, 11)));
  m_loadMode = addControl(new LoadModeMenu(Rect(195, 1, 58, 62)));

  auto isDualEB = Application::get().getPresetManager()->getEditBuffer()->isDual();

  if(isDualEB && HWUIHelper::isLoadToPartActive())
    m_presets = addControl(new LoadToPartPresetList(Rect(64, 0, 128, 63), true, getPresetPartSelection(VoiceGroup::I),
                                                    getPresetPartSelection(VoiceGroup::II)));
  else
    m_presets = addControl(new PresetList({ 64, 0, 128, 63 }, true));

  m_presets->setBankFocus();
}

void PresetManagerLayout::setupBankStore()
{
  if(getStoreModeData() == nullptr)
  {
    setStoreModeData(std::make_unique<StoreModeData>());
  }
  addControl(new BankAndPresetNumberLabel(Rect(0, 0, 64, 14)));
  addControl(new InvertedLabel("Store", Rect(8, 26, 48, 12)))->setHighlight(true);
  addControl(new AnyParameterLockedIndicator(Rect(244, 2, 10, 11)));

  addControl(new UndoIndicator(Rect { 5, 14, 15, 5 }));

  m_menu = addControl(new AppendOverwriteInsertButtonMenu(*this, Rect(195, 1, 58, 62)));
  m_presets = addControl(new PresetListSelectStorePosition(Rect(64, 0, 128, 63), true, getStoreModeData()));
  m_presets->setBankFocus();
}

void PresetManagerLayout::setupPresetFocus()
{
  m_bankButton = addControl(new BankButton({ 3, 36, 58, 57 }, false));

  switch(m_focusAndMode.mode)
  {
    case UIMode::Edit:
      setupPresetEdit();
      break;

    case UIMode::Select:
      setupPresetSelect();
      break;

    case UIMode::Store:
      setupPresetStore();
      break;
  }
}

void PresetManagerLayout::setupPresetEdit()
{
  if(getStoreModeData() != nullptr)
  {
    setStoreModeData(nullptr);
  }
  auto selectedBank = Application::get().getPresetManager()->getSelectedBank();
  addControl(new BankAndPresetNumberLabel(Rect(0, 0, 64, 14)));
  addControl(new InvertedLabel("Edit", Rect(8, 26, 48, 12)))->setHighlight(true);
  m_presets = addControl(new PresetList(Rect(64, 0, 128, 63), true));
  addControl(new AnyParameterLockedIndicator(Rect(244, 2, 10, 11)));

  addControl(new UndoIndicator(Rect { 5, 14, 15, 5 }));

  if(selectedBank && !selectedBank->empty())
  {
    m_menu = addControl(new PresetEditButtonMenu(Rect(195, 1, 58, 62)));
  }
  else
  {
    addControl(new Button("", Buttons::BUTTON_D));
  }
}

void PresetManagerLayout::setupPresetSelect()
{
  if(getStoreModeData() != nullptr)
  {
    setStoreModeData(nullptr);
  }
  addControl(new BankAndPresetNumberLabel(Rect(0, 0, 64, 14)));
  addControl(new NumPresetsInBankLabel(Rect(192, 1, 64, 14)));
  addControl(new AnyParameterLockedIndicator(Rect(244, 2, 10, 11)));
  m_loadMode = addControl(new LoadModeMenu(Rect(195, 1, 58, 62)));

  auto isDualEditBuffer = Application::get().getPresetManager()->getEditBuffer()->getType() != SoundType::Single;

  if(HWUIHelper::isLoadToPartActive() && isDualEditBuffer)
    m_presets = addControl(new LoadToPartPresetList(Rect(64, 0, 128, 63), true, getPresetPartSelection(VoiceGroup::I),
                                                    getPresetPartSelection(VoiceGroup::II)));
  else
    m_presets = addControl(new PresetList(Rect(64, 0, 128, 63), true));

  addControl(new VoiceGroupIndicator(Rect(2, 15, 16, 16)));
  addControl(new UndoIndicator(Rect(27, 16, 10, 8)));
}

void PresetManagerLayout::setupPresetStore()
{
  if(getStoreModeData() == nullptr)
  {
    setStoreModeData(std::make_unique<StoreModeData>());
  }
  addControl(new BankAndPresetNumberLabel(Rect(0, 0, 64, 14)));
  addControl(new InvertedLabel("Store", Rect(8, 26, 48, 12)))->setHighlight(true);
  addControl(new AnyParameterLockedIndicator(Rect(244, 2, 10, 11)));

  addControl(new UndoIndicator(Rect { 5, 14, 15, 5 }));

  m_presets = addControl(new PresetListSelectStorePosition(Rect(64, 0, 128, 63), true, getStoreModeData()));
  m_menu = addControl(new AppendOverwriteInsertButtonMenu(*this, Rect(195, 1, 58, 62)));
}

bool PresetManagerLayout::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  if(m_loadMode)
    if(m_loadMode->onButton(i, down, modifiers))
      return true;

  if(m_bankButton)
    if(m_bankButton->onButton(i, down, modifiers))
      return true;

  if(!down)
  {
    removeButtonRepeat();
  }

  if(down)
  {
    auto &app = Application::get();
    auto hwui = app.getHWUI();

    switch(i)
    {
      case Buttons::BUTTON_A:
        if(m_focusAndMode.focus == UIFocus::Banks)
          hwui->undoableSetFocusAndMode(FocusAndMode(UIFocus::Presets));
        else
          hwui->undoableSetFocusAndMode(FocusAndMode(UIFocus::Banks));
        break;

      case Buttons::BUTTON_B:
      case Buttons::BUTTON_C:
        m_presets->onButton(i, down, modifiers);

        return true;

      case Buttons::BUTTON_D:
        if(m_menu)
        {
          if(modifiers[SHIFT] == 1)
          {
            m_menu->antiToggle();
          }
          else
          {
            m_menu->toggle();
          }
        }

        return true;

      case Buttons::BUTTON_STORE:
        if(m_focusAndMode.mode == UIMode::Store)
          hwui->undoableSetFocusAndMode(m_oldFocusAndMode);
        else
          hwui->undoableSetFocusAndMode({ UIFocus::Presets, UIMode::Store, UIDetail::Init });
        break;

      case Buttons::BUTTON_EDIT:
        if(m_focusAndMode.mode == UIMode::Edit)
          hwui->undoableSetFocusAndMode(UIMode::Select);
        else
          hwui->undoableSetFocusAndMode(UIMode::Edit);
        break;

      case Buttons::BUTTON_PRESET:
        if(m_focusAndMode.mode == UIMode::Store)
          hwui->undoableSetFocusAndMode({ UIMode::Select });
        else
          hwui->undoableSetFocusAndMode(m_oldFocusAndMode);
        break;

      case Buttons::BUTTON_INFO:
        hwui->undoableSetFocusAndMode(UIMode::Info);
        break;

      case Buttons::BUTTON_ENTER:
        if(m_menu)
          m_menu->doAction();
        else if(m_focusAndMode.detail == UIDetail::LoadToPart)
        {
          return m_presets->onButton(i, down, modifiers);
        }
        else if(m_focusAndMode.mode == UIMode::Select)
        {
          loadSelectedPresetAccordingToLoadType();
        }
    }
  }

  return super::onButton(i, down, modifiers);
}

bool PresetManagerLayout::onRotary(int inc, ButtonModifiers modifiers)
{
  m_presets->onRotary(inc, modifiers);
  return Layout::onRotary(inc, modifiers);
}

bool PresetManagerLayout::animateSelectedPreset(std::function<void()> cb)
{
  return m_presets->animateSelectedPreset(std::move(cb));
}

void PresetManagerLayout::animateSelectedPresetIfInLoadPartMode(std::function<void()> cb)
{
  if(HWUIHelper::isLoadToPartActive())
    m_presets->animateSelectedPreset(std::move(cb));
  else
    cb();
}

std::pair<size_t, size_t> PresetManagerLayout::getSelectedPosition() const
{
  if(m_presets)
    return m_presets->getSelectedPosition();

  return {};
}

std::unique_ptr<StoreModeData> &PresetManagerLayout::getStoreModePtr()
{
  static std::unique_ptr<StoreModeData> s_storeModeData;
  return s_storeModeData;
}

PresetPartSelection *PresetManagerLayout::getPresetPartSelection(VoiceGroup vg)
{
  static std::array<PresetPartSelection, 2> s_partLoad { PresetPartSelection { VoiceGroup::I },
                                                         PresetPartSelection { VoiceGroup::II } };
  return &s_partLoad[static_cast<int>(vg)];
}

StoreModeData *PresetManagerLayout::getStoreModeData()
{

  return getStoreModePtr().get();
}

void PresetManagerLayout::setStoreModeData(std::unique_ptr<StoreModeData> ptr)
{
  getStoreModePtr() = std::move(ptr);
}

void PresetManagerLayout::loadSelectedPresetAccordingToLoadType()
{
  auto pm = getPresetManager();
  auto eb = pm->getEditBuffer();
  auto currentVoiceGroup = Application::get().getHWUI()->getCurrentVoiceGroup();

  if(auto bank = pm->getSelectedBank())
  {
    if(auto selPreset = bank->getSelectedPreset())
    {
      auto directLoadSetting = Application::get().getSettings()->getSetting<DirectLoadSetting>();
      auto loadToPartActive = HWUIHelper::isLoadToPartActive();

      switch(selPreset->getType())
      {
        case SoundType::Single:
          eb->undoableLoadSelectedPreset(currentVoiceGroup);
          animateSelectedPresetIfInLoadPartMode([]() {});
          break;
        case SoundType::Layer:
        case SoundType::Split:
          if(loadToPartActive)
          {
            auto load = getPresetPartSelection(currentVoiceGroup);
            eb->undoableLoadToPart(load->m_preset, load->m_voiceGroup, currentVoiceGroup);
            animateSelectedPresetIfInLoadPartMode([]() {});
          }
          else
          {
            eb->undoableLoadSelectedPreset(currentVoiceGroup);
            animateSelectedPresetIfInLoadPartMode([]() {});
          }
          break;
      }
    }
  }
}

#include <utility>

#include <Application.h>
#include <device-settings/AutoLoadSelectedPreset.h>
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
#include <proxies/hwui/panel-unit/PanelUnit.h>
#include <functional>
#include <memory>
#include <vector>
#include <proxies/hwui/panel-unit/boled/undo/UndoIndicator.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/AnyParameterLockedIndicator.h>

PresetManagerLayout::PresetManagerLayout(FocusAndMode focusAndMode)
    : super(Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled())
    , m_focusAndMode(focusAndMode)
{
  setup();
}

PresetManagerLayout::~PresetManagerLayout()
{
  m_focusAndMode.mode = UIMode::Edit;
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
  m_autoLoad = nullptr;
  m_presets = nullptr;
  m_numPresetsInBank = nullptr;
  m_bankAndPresetNumberLabel = nullptr;

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

void PresetManagerLayout::setupBankFocus()
{
  addControl(new Button("Bank", Buttons::BUTTON_A))->setHighlight(true);

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
  addControl(new BankAndPresetNumberLabel(Rect(0, 1, 64, 14)));
  addControl(new InvertedLabel("Edit", Rect(8, 26, 48, 12)))->setHighlight(true);
  addControl(new UndoIndicator(Rect(4, 15, 10, 5)));
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
  addControl(new BankAndPresetNumberLabel(Rect(0, 1, 64, 14)));
  addControl(new NumBanksLabel(Rect(208, 1, 32, 14)))->setHighlight(false);
  addControl(new UndoIndicator(Rect(4, 15, 10, 5)));
  addControl(new AnyParameterLockedIndicator(Rect(244, 2, 10, 11)));
  m_autoLoad = addControl(new Button("Direct Load", BUTTON_D));
  m_presets = addControl(new PresetList(Rect(64, 0, 128, 63), true));
  m_presets->setBankFocus();
  Application::get().getSettings()->getSetting<AutoLoadSelectedPreset>()->onChange(
      sigc::mem_fun(this, &PresetManagerLayout::updateAutoLoadButton));
}

void PresetManagerLayout::setupBankStore()
{
  if(getStoreModeData() == nullptr)
  {
    setStoreModeData(std::move(std::make_unique<StoreModeData>()));
  }
  addControl(new BankAndPresetNumberLabel(Rect(0, 1, 64, 14)));
  addControl(new InvertedLabel("Store", Rect(8, 26, 48, 12)))->setHighlight(true);
  addControl(new AnyParameterLockedIndicator(Rect(244, 2, 10, 11)));
  addControl(new UndoIndicator(Rect(4, 15, 10, 5)));
  m_menu = addControl(new AppendOverwriteInsertButtonMenu(*this, Rect(195, 1, 58, 62)));
  m_presets = addControl(new PresetListSelectStorePosition(Rect(64, 0, 128, 63), true, getStoreModeData()));
  m_presets->setBankFocus();
}

void PresetManagerLayout::setupPresetFocus()
{
  addControl(new Button("Bank", Buttons::BUTTON_A));

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
  addControl(new BankAndPresetNumberLabel(Rect(0, 1, 64, 14)));
  addControl(new InvertedLabel("Edit", Rect(8, 26, 48, 12)))->setHighlight(true);
  m_presets = addControl(new PresetList(Rect(64, 0, 128, 63), true));
  addControl(new AnyParameterLockedIndicator(Rect(244, 2, 10, 11)));
  addControl(new UndoIndicator(Rect(4, 15, 10, 5)));

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
  m_bankAndPresetNumberLabel = addControl(new BankAndPresetNumberLabel(Rect(0, 1, 64, 14)));
  m_numPresetsInBank = addControl(new NumPresetsInBankLabel(Rect(192, 1, 64, 14)));
  addControl(new AnyParameterLockedIndicator(Rect(244, 2, 10, 11)));
  m_autoLoad = addControl(new Button("Direct Load", BUTTON_D));
  m_presets = addControl(new PresetList(Rect(64, 0, 128, 63), true));
  addControl(new UndoIndicator(Rect(4, 15, 10, 5)));
  Application::get().getSettings()->getSetting<AutoLoadSelectedPreset>()->onChange(
      sigc::mem_fun(this, &PresetManagerLayout::updateAutoLoadButton));
}

void PresetManagerLayout::setupPresetStore()
{
  if(getStoreModeData() == nullptr)
  {
    setStoreModeData(std::move(std::make_unique<StoreModeData>()));
  }
  m_bankAndPresetNumberLabel = addControl(new BankAndPresetNumberLabel(Rect(0, 1, 64, 14)));
  addControl(new InvertedLabel("Store", Rect(8, 26, 48, 12)))->setHighlight(true);
  addControl(new AnyParameterLockedIndicator(Rect(244, 2, 10, 11)));
  addControl(new UndoIndicator(Rect(4, 15, 10, 5)));
  m_presets = addControl(new PresetListSelectStorePosition(Rect(64, 0, 128, 63), true, getStoreModeData()));
  m_menu = addControl(new AppendOverwriteInsertButtonMenu(*this, Rect(195, 1, 58, 62)));
}

bool PresetManagerLayout::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  if(!down)
  {
    removeButtonRepeat();
  }

  if(down)
  {
    auto &app = Application::get();
    auto hwui = app.getHWUI();
    auto pm = app.getPresetManager();

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
        installButtonRepeat([=]() { m_presets->onButton(i, down, modifiers); });

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
        else if(m_autoLoad)
        {
          Application::get().getSettings()->getSetting<AutoLoadSelectedPreset>()->toggle();
        }

        return true;

      case Buttons::BUTTON_STORE:
        if(m_focusAndMode.mode == UIMode::Store)
          hwui->undoableSetFocusAndMode(UIMode::Select);
        else
          hwui->undoableSetFocusAndMode(UIMode::Store);
        break;

      case Buttons::BUTTON_EDIT:
        if(m_focusAndMode.mode == UIMode::Edit)
          hwui->undoableSetFocusAndMode(UIMode::Select);
        else
          hwui->undoableSetFocusAndMode(UIMode::Edit);
        break;

      case Buttons::BUTTON_PRESET:
        if(m_focusAndMode.focus == UIFocus::Presets && m_focusAndMode.mode == UIMode::Select)
          hwui->undoableSetFocusAndMode(UIFocus::Parameters);
        else
          hwui->undoableSetFocusAndMode({ UIFocus::Presets, UIMode::Select });
        break;

      case Buttons::BUTTON_INFO:
        hwui->undoableSetFocusAndMode(UIMode::Info);
        break;

      case Buttons::BUTTON_ENTER:
        if(m_menu)
          m_menu->doAction();
        else if(m_focusAndMode.mode == UIMode::Select)
          pm->getEditBuffer()->undoableLoadSelectedPreset();
    }
  }

  return super::onButton(i, down, modifiers);
}

bool PresetManagerLayout::onRotary(int inc, ButtonModifiers modifiers)
{
  m_presets->onRotary(inc, modifiers);
  return DFBLayout::onRotary(inc, modifiers);
}

void PresetManagerLayout::updateAutoLoadButton(const Setting *setting)
{
  if(m_autoLoad)
  {
    const auto *s = dynamic_cast<const AutoLoadSelectedPreset *>(setting);
    m_autoLoad->setHighlight(s->get());
  }
}

bool PresetManagerLayout::animateSelectedPreset(std::function<void()> cb)
{
  return m_presets->animateSelectedPreset(std::move(cb));
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

StoreModeData *PresetManagerLayout::getStoreModeData()
{

  return getStoreModePtr().get();
}

void PresetManagerLayout::setStoreModeData(std::unique_ptr<StoreModeData> ptr)
{
  getStoreModePtr() = std::move(ptr);
}

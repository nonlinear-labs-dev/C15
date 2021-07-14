#include "AppendOverwriteInsertButtonMenu.h"
#include <proxies/hwui/panel-unit/boled/preset-screens/PresetManagerLayout.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/RenamePresetLayout.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/panel-unit/boled/BOLED.h>
#include <proxies/hwui/panel-unit/PanelUnit.h>
#include "Application.h"
#include "device-settings/Settings.h"
#include <presets/PresetManager.h>
#include <presets/Bank.h>
#include <presets/Preset.h>
#include <presets/EditBuffer.h>
#include <http/UndoScope.h>
#include <use-cases/PresetUseCases.h>

AppendOverwriteInsertButtonMenu::AppendOverwriteInsertButtonMenu(PresetManagerLayout& parent, const Rect& rect)
    : super(rect)
    , m_parent(parent)
{
  Application::get().getPresetManager()->onNumBanksChanged(
      sigc::hide<0>(mem_fun(this, &AppendOverwriteInsertButtonMenu::buildMenu)));
}

void AppendOverwriteInsertButtonMenu::buildMenu()
{
  clear();

  auto pm = Application::get().getPresetManager();
  addButton("Append", std::bind(&AppendOverwriteInsertButtonMenu::executeAction, this));
  if(pm->getSelectedBank() && pm->getSelectedBank()->getNumPresets() != 0)
    addButton("Overwrite", std::bind(&AppendOverwriteInsertButtonMenu::executeAction, this));
  addButton("Insert", std::bind(&AppendOverwriteInsertButtonMenu::executeAction, this));

  auto setting = Application::get().getSettings()->getSetting<PresetStoreModeSetting>()->get();
  selectButton(enumToIndex(setting));
}

AppendOverwriteInsertButtonMenu::~AppendOverwriteInsertButtonMenu() = default;

void AppendOverwriteInsertButtonMenu::selectButton(size_t i)
{
  super::selectButton(i);
  Application::get().getSettings()->getSetting<PresetStoreModeSetting>()->set(indexToEnum(i));
}

PresetStoreModeSettings AppendOverwriteInsertButtonMenu::indexToEnum(size_t i) const
{
  return static_cast<PresetStoreModeSettings>(i);
}

size_t AppendOverwriteInsertButtonMenu::enumToIndex(PresetStoreModeSettings i) const
{
  return static_cast<size_t>(i);
}

bool AppendOverwriteInsertButtonMenu::animate()
{
  auto currentLayout = Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().getBaseLayout();

  if(auto presetManagerLayout = dynamic_cast<PresetManagerLayout*>(currentLayout.get()))
  {
    return presetManagerLayout->animateSelectedPreset(
        [] { Application::get().getHWUI()->undoableSetFocusAndMode(FocusAndMode(UIFocus::Presets, UIMode::Select)); });
  }
  else
  {
    Application::get().getHWUI()->undoableSetFocusAndMode(FocusAndMode(UIFocus::Presets, UIMode::Select));
  }

  return false;
}

void AppendOverwriteInsertButtonMenu::executeAction()
{
  auto pm = Application::get().getPresetManager();
  PresetManagerUseCases useCases(pm);
  auto actionPosition = m_parent.getSelectedPosition();

  if(auto selectedBank = pm->getBankAt(actionPosition.first))
  {
    if(auto selectedPreset = selectedBank->getPresetAt(actionPosition.second))
    {
      Application::get().getUndoScope()->resetCukooTransaction();

      auto setting = Application::get().getSettings()->getSetting<PresetStoreModeSetting>()->get();
      bool modified = pm->getEditBuffer()->isModified();

      switch(setting)
      {
        case PresetStoreModeSettings::PRESET_STORE_MODE_APPEND:
          useCases.appendPreset(selectedBank);
          if(modified)
          {
            pushRenameScreen();
          }
          break;

        case PresetStoreModeSettings::PRESET_STORE_MODE_INSERT:
          useCases.insertPreset(selectedBank, selectedBank->getPresetPosition(selectedPreset->getUuid()) + 1);
          if(modified)
            pushRenameScreen();
          else
            animate();
          break;

        case PresetStoreModeSettings::PRESET_STORE_MODE_OVERWRITE:
          useCases.overwritePreset(selectedPreset);
          animate();
          break;
      }
    }
    else
    {
      useCases.insertPreset(selectedBank, 0);
      pushRenameScreen();
    }
  }
  else
  {
    useCases.createBankAndStoreEditBuffer();
  }
}

void AppendOverwriteInsertButtonMenu::pushRenameScreen()
{
  Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().setOverlay(new RenamePresetLayout(
      [=](const Glib::ustring& newName) {
        if(auto bank = Application::get().getPresetManager()->getSelectedBank())
        {
          if(auto preset = bank->getSelectedPreset())
          {
            PresetUseCases useCases(preset);
            useCases.rename(newName);
          }
        }
        animate();
      },
      [=]() { animate(); }));
}
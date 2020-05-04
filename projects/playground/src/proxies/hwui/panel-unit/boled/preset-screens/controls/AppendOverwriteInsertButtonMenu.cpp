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

AppendOverwriteInsertButtonMenu::~AppendOverwriteInsertButtonMenu()
{
}

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
  auto actionPosition = m_parent.getSelectedPosition();

  if(auto bank = pm->getBankAt(actionPosition.first))
  {
    if(auto tgtPreset = bank->getPresetAt(actionPosition.second))
    {
      Application::get().getUndoScope()->resetCukooTransaction();

      auto setting = Application::get().getSettings()->getSetting<PresetStoreModeSetting>()->get();
      bool modified = pm->getEditBuffer()->isModified();

      switch(setting)
      {
        case PresetStoreModeSettings::PRESET_STORE_MODE_APPEND:
          append(bank, modified);
          break;

        case PresetStoreModeSettings::PRESET_STORE_MODE_INSERT:
          insert(bank, tgtPreset, modified);
          break;

        case PresetStoreModeSettings::PRESET_STORE_MODE_OVERWRITE:
          overwrite(bank, tgtPreset, modified);
          break;
      }
    }
    else
    {
      insertPreset(bank, 0, true);
      pushRenameScreen();
    }
  }
  else
  {
    createBankAndStore();
  }
}

void AppendOverwriteInsertButtonMenu::createBankAndStore()
{
  auto pm = Application::get().getPresetManager();
  auto& scope = pm->getUndoScope();
  auto transactionScope = scope.startTransaction("Create Bank and Store Preset");
  auto transaction = transactionScope->getTransaction();
  auto bank = pm->addBank(transaction);
  auto preset = bank->appendAndLoadPreset(transaction, std::make_unique<Preset>(bank, *pm->getEditBuffer()));
  bank->selectPreset(transaction, preset->getUuid());
  pm->selectBank(transaction, bank->getUuid());
  Application::get().getHWUI()->setFocusAndMode(FocusAndMode(UIFocus::Presets, UIMode::Select));
}

void AppendOverwriteInsertButtonMenu::append(Bank* bank, bool modified)
{
  appendPreset(bank, modified);

  if(modified)
    pushRenameScreen();
  else
    animate();
}

void AppendOverwriteInsertButtonMenu::insert(Bank* bank, Preset* tgtPreset, bool modified)
{
  insertPreset(bank, bank->getPresetPosition(tgtPreset->getUuid()) + 1, modified);

  if(modified)
    pushRenameScreen();
  else
    animate();
}

void AppendOverwriteInsertButtonMenu::overwrite(Bank* bank, Preset* tgtPreset, bool modified)
{
  auto pm = Application::get().getPresetManager();
  auto scope = pm->getUndoScope().startTransaction("Overwrite preset '%0'", tgtPreset->getName());
  auto transaction = scope->getTransaction();

  bool loaded
      = tgtPreset->getUuid() == Application::get().getPresetManager()->getEditBuffer()->getUUIDOfLastLoadedPreset();

  tgtPreset = overwritePreset(transaction, tgtPreset);

  if(loaded)
  {
    animate();
  }
  else if(modified)
  {
    tgtPreset->guessName(transaction);
    pushRenameScreen();
  }
  else
  {
    animate();
  }
}

void AppendOverwriteInsertButtonMenu::pushRenameScreen()
{
  auto layout = new RenamePresetLayout(
      [=](const Glib::ustring& newName) {
        if(auto bank = Application::get().getPresetManager()->getSelectedBank())
        {
          if(auto preset = bank->getSelectedPreset())
          {
            if(preset->getName() != newName)
            {
              auto scope = Application::get().getUndoScope()->startTransaction("Rename preset");
              preset->setName(scope->getTransaction(), newName);
            }
          }
        }
        animate();
      },
      [=]() { animate(); });

  Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().setOverlay(layout);
}

Preset* AppendOverwriteInsertButtonMenu::overwritePreset(Preset* preset)
{
  auto scope = Application::get().getUndoScope()->startTransaction("Overwrite preset '%0'", preset->getName());
  return overwritePreset(scope->getTransaction(), preset);
}

Preset* AppendOverwriteInsertButtonMenu::overwritePreset(UNDO::Transaction* transaction, Preset* preset)
{
  auto pm = Application::get().getPresetManager();

  if(auto bank = dynamic_cast<Bank*>(preset->getParent()))
  {
    preset->copyFrom(transaction, pm->getEditBuffer());
    bank->selectPreset(transaction, preset->getUuid());
    pm->selectBank(transaction, bank->getUuid());
  }
  return preset;
}

void AppendOverwriteInsertButtonMenu::insertPreset(Bank* bank, size_t pos, bool modified)
{
  auto pm = Application::get().getPresetManager();
  auto scope = Application::get().getUndoScope()->startTransaction("Insert preset at position %0", pos + 1);
  auto transaction = scope->getTransaction();
  auto preset
      = bank->insertAndLoadPreset(scope->getTransaction(), pos, std::make_unique<Preset>(bank, *pm->getEditBuffer()));

  if(modified)
    preset->guessName(transaction);

  bank->selectPreset(scope->getTransaction(), preset->getUuid());
  pm->selectBank(transaction, bank->getUuid());
}

void AppendOverwriteInsertButtonMenu::appendPreset(Bank* bank, bool modified)
{
  insertPreset(bank, bank->getNumPresets(), modified);
}

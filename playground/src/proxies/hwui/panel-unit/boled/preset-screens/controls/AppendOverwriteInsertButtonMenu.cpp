#include "AppendOverwriteInsertButtonMenu.h"
#include <proxies/hwui/panel-unit/boled/preset-screens/PresetManagerLayout.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/RenamePresetLayout.h>
#include <proxies/hwui/HWUI.h>
#include "Application.h"
#include "device-settings/Settings.h"
#include <presets/PresetManager.h>
#include <presets/PresetBank.h>
#include <presets/Preset.h>
#include <presets/EditBuffer.h>
#include <http/UndoScope.h>

AppendOverwriteInsertButtonMenu::AppendOverwriteInsertButtonMenu(PresetManagerLayout &parent, const Rect &rect)
    : super(rect)
    , m_parent(parent)
{
  Application::get().getPresetManager()->onNumBanksChanged(
      sigc::hide<0>(mem_fun(this, &AppendOverwriteInsertButtonMenu::buildMenu)));
}

void AppendOverwriteInsertButtonMenu::buildMenu()
{
  clear();
  clearActions();

  auto pm = Application::get().getPresetManager();
  addButton("Append", bind(&AppendOverwriteInsertButtonMenu::executeAction, this));
  if(pm->getSelectedBank() && pm->getSelectedBank()->getNumPresets() != 0)
    addButton("Overwrite", bind(&AppendOverwriteInsertButtonMenu::executeAction, this));
  addButton("Insert", bind(&AppendOverwriteInsertButtonMenu::executeAction, this));

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
  return m_parent.animateSelectedPreset(
      []() { Application::get().getHWUI()->undoableSetFocusAndMode(UIMode::Select); });
}

void AppendOverwriteInsertButtonMenu::executeAction()
{
  auto pm = Application::get().getPresetManager();
  auto actionPosition = m_parent.getSelectedPosition();

  if(auto bank = pm->getBank(actionPosition.first))
  {
    if(auto tgtPreset = bank->getPreset(actionPosition.second))
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
  auto &scope = pm->getUndoScope();
  auto transactionScope = scope.startTransaction("Create Bank and Store Preset");
  auto transaction = transactionScope->getTransaction();
  pm->addBank(transaction, true);
  auto bank = pm->getSelectedBank();
  auto preset = Preset::createPreset(bank.get());
  bank->undoableCopyAndPrependPreset(transaction, pm->getEditBuffer());
  bank->undoableEnsurePresetSelection(transaction);
  Application::get().getHWUI()->setFocusAndMode(FocusAndMode(UIFocus::Presets, UIMode::Select));
}

void AppendOverwriteInsertButtonMenu::append(shared_ptr<PresetBank> bank, bool modified)
{
  appendPreset(std::move(bank), modified);

  if(modified)
    pushRenameScreen();
  else
    animate();
}

void AppendOverwriteInsertButtonMenu::insert(shared_ptr<PresetBank> bank, shared_ptr<Preset> tgtPreset, bool modified)
{
  insertPreset(bank, bank->getPresetPosition(tgtPreset->getUuid()) + 1, modified);

  if(modified)
    pushRenameScreen();
  else
    animate();
}

void AppendOverwriteInsertButtonMenu::overwrite(shared_ptr<PresetBank> bank, shared_ptr<Preset> tgtPreset,
                                                bool modified)
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
    pm->getSelectedPreset()->guessName(transaction);
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
      [=](const Glib::ustring &newName) {
        if(auto bank = Application::get().getPresetManager()->getSelectedBank())
        {
          const auto &uuid = bank->getSelectedPreset();

          if(auto preset = bank->getPreset(uuid))
          {
            auto scope = Application::get().getUndoScope()->startTransaction("Rename preset");
            preset->undoableSetName(scope->getTransaction(), newName);
          }
        }
        animate();
      },
      [=]() { animate(); });

  Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().setOverlay(layout);
}

shared_ptr<Preset> AppendOverwriteInsertButtonMenu::overwritePreset(shared_ptr<Preset> preset)
{
  auto scope = Application::get().getUndoScope()->startTransaction("Overwrite preset '%0'", preset->getName());
  return overwritePreset(scope->getTransaction(), preset);
}

shared_ptr<Preset>
    AppendOverwriteInsertButtonMenu::overwritePreset(AppendOverwriteInsertButtonMenu::tTransactionPtr transaction,
                                                     shared_ptr<Preset> preset)
{
  auto pm = Application::get().getPresetManager();

  if(auto bank = preset->getBank())
  {
    size_t anchorPos = bank->getPresetPosition(preset->getUuid());
    bank->undoableOverwritePreset(transaction, anchorPos, pm->getEditBuffer());
    bank->getPreset(anchorPos)->undoableSelect(transaction);
    bank->undoableSelect(transaction);
    return bank->getPreset(anchorPos);
  }

  return preset;
}

void AppendOverwriteInsertButtonMenu::insertPreset(shared_ptr<PresetBank> bank, size_t pos, bool modified)
{
  auto scope = Application::get().getUndoScope()->startTransaction("Insert preset at position %0", pos + 1);
  auto transaction = scope->getTransaction();
  bank->undoableInsertPreset(transaction, pos);
  overwritePreset(transaction, bank->getPreset(pos));

  if(modified)
    bank->getPreset(pos)->guessName(transaction);
}

void AppendOverwriteInsertButtonMenu::appendPreset(shared_ptr<PresetBank> bank, bool modified)
{
  insertPreset(bank, bank->getNumPresets(), modified);
}

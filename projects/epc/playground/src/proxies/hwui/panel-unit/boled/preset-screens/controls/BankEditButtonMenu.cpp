#include <Application.h>
#include <clipboard/Clipboard.h>
#include <libundo/undo/Scope.h>
#include <http/UndoScope.h>
#include <presets/BankActions.h>
#include <presets/Bank.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/BankEditButtonMenu.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/RenameBankLayout.h>
#include <tools/FileSystem.h>
#include <serialization/PresetBankSerializer.h>
#include <xml/FileOutStream.h>
#include <proxies/hwui/panel-unit/boled/SplashLayout.h>
#include <proxies/hwui/panel-unit/boled/file/RenameExportLayout.h>
#include <proxies/hwui/panel-unit/boled/setup/USBStickAvailableView.h>
#include <proxies/hwui/panel-unit/boled/file/FileDialogLayout.h>
#include <device-info/DateTimeInfo.h>
#include <xml/VersionAttribute.h>
#include <tools/TimeTools.h>
#include <nltools/GenericScopeGuard.h>
#include <device-settings/Settings.h>
#include <device-settings/ExternalMidiEnabledSetting.h>

static size_t s_lastSelectedButton = 0;

BankEditButtonMenu::BankEditButtonMenu(const Rect& rect)
    : super(rect)
{
  Application::get().getClipboard()->onClipboardChanged(
      mem_fun(this, &BankEditButtonMenu::rebuildMenuOnClipboardChange));
  Application::get().getPresetManager()->onNumBanksChanged(mem_fun(this, &BankEditButtonMenu::rebuildMenu));

  Application::get().getPresetManager()->onMidiBankSelectionHappened(
      mem_fun(this, &BankEditButtonMenu::onMidiSelectionChanged));
}

void BankEditButtonMenu::rebuildMenuOnClipboardChange()
{
  rebuildMenu(Application::get().getPresetManager()->getNumBanks());
}

void BankEditButtonMenu::rebuildMenu(size_t numBanks)
{
  clear();

  if(numBanks != 0)
    rebuildFullMenu();
  else
    rebuildNoBankAvailableMenu();

  correctMenuSelection();
  selectButton(s_lastSelectedButton);
}

void BankEditButtonMenu::onMidiSelectionChanged(Uuid bankUuid)
{
  auto numbanks = Application::get().getPresetManager()->getNumBanks();
  rebuildMenu(numbanks);
}

void BankEditButtonMenu::rebuildFullMenu()
{
  addButton("New", std::bind(&BankEditButtonMenu::newBank, this));

  if(USBStickAvailableView::usbIsReady())
  {
    addButton("Export ...", std::bind(&BankEditButtonMenu::exportBank, this));
    addButton("Import ...", std::bind(&BankEditButtonMenu::importBank, this));
  }

  addButton("Rename", std::bind(&BankEditButtonMenu::renameBank, this));
  addButton("Copy", std::bind(&BankEditButtonMenu::copyBank, this));

  if(Application::get().getClipboard()->hasContent())
  {
    addButton("Paste", std::bind(&BankEditButtonMenu::pasteBank, this));
  }

  addButton("Delete", std::bind(&BankEditButtonMenu::deleteBank, this));

  if(Application::get().getSettings()->getSetting<ExternalMidiEnabledSetting>()->get())
  {
    if(Application::get().getPresetManager()->findMidiSelectedBank()
       != Application::get().getPresetManager()->getSelectedBank())
    {
      addButton("Rec. Midi PC", std::bind(&BankEditButtonMenu::selectMidi, this));
    }
    else
    {
      addButton("Rem. Midi PC", std::bind(&BankEditButtonMenu::removeMidi, this));
    }
  }

  addButton("Move Left", std::bind(&BankEditButtonMenu::moveLeft, this));
  addButton("Move Right", std::bind(&BankEditButtonMenu::moveRight, this));
}

void BankEditButtonMenu::rebuildNoBankAvailableMenu()
{

  addButton("New", std::bind(&BankEditButtonMenu::newBank, this));

  if(USBStickAvailableView::usbIsReady())
  {
    addButton("Import ...", std::bind(&BankEditButtonMenu::importBank, this));
  }

  if(Application::get().getClipboard()->hasContent())
  {
    addButton("Paste", std::bind(&BankEditButtonMenu::pasteBank, this));
  }
}

void BankEditButtonMenu::selectButton(size_t i)
{
  s_lastSelectedButton = i;
  super::selectButton(i);
}

void BankEditButtonMenu::newBank()
{
  auto pm = Application::get().getPresetManager();
  auto scope = pm->getUndoScope().startTransaction("New bank");
  auto transaction = scope->getTransaction();
  auto newBank = pm->addBank(transaction);
  auto pos = pm->calcDefaultBankPositionFor(newBank);
  newBank->setX(transaction, to_string(pos.first));
  newBank->setY(transaction, to_string(pos.second));
  pm->selectBank(transaction, newBank->getUuid());
  auto layout = new RenameBankLayout(transaction, [](auto transaction) {
    Application::get().getHWUI()->undoableSetFocusAndMode(transaction, FocusAndMode(UIFocus::Presets, UIMode::Select));
  });
  Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().setOverlay(layout);
}

BankEditButtonMenu::FileInfos BankEditButtonMenu::extractFileInfos(const std::filesystem::directory_entry& file)
{
  return FileInfos { file };
}

bool BankEditButtonMenu::applicableBackupFilesFilter(const std::filesystem::directory_entry& term)
{
  auto fileName = term.path().filename().string();
  std::string end = ".xml";
  return !std::equal(end.rbegin(), end.rend(), fileName.rbegin());
}

void BankEditButtonMenu::importBankFromPath(const std::filesystem::directory_entry& file)
{
  auto hwui = Application::get().getHWUI();
  if(file != std::filesystem::directory_entry())
  {
    auto fileInfos = extractFileInfos(file);

    hwui->getPanelUnit().getEditPanel().getBoled().setOverlay(new SplashLayout());

    FileInStream stream(fileInfos.filePath, false);
    SplashLayout::addStatus("Importing " + fileInfos.fileName);
    auto& bankActions = Application::get().getPresetManager()->findActionManager<BankActions>();
    bankActions.importBank(stream, "0", "0", fileInfos.fileName);
  }
  hwui->getPanelUnit().getEditPanel().getBoled().resetOverlay();
  hwui->getPanelUnit().setupFocusAndMode({ UIFocus::Presets, UIMode::Select });
}

void BankEditButtonMenu::importBank()
{
  Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().reset(
      new FileDialogLayout(&BankEditButtonMenu::applicableBackupFilesFilter, &BankEditButtonMenu::importBankFromPath,
                           "Select Bank for Import"));
}

Glib::ustring BankEditButtonMenu::createValidOutputPath(const Glib::ustring& bankName)
{
  auto fileName = FileSystem::findSuitableFileName(bankName, "/mnt/usb-stick/", 0);
  return "/mnt/usb-stick/" + fileName + ".xml";
}

void BankEditButtonMenu::exportBank()
{
  auto& panelunit = Application::get().getHWUI()->getPanelUnit();
  auto& boled = panelunit.getEditPanel().getBoled();

  if(auto selBank = Application::get().getPresetManager()->getSelectedBank())
  {
    boled.setOverlay(new RenameExportLayout(selBank, [](Glib::ustring newExportName, auto bank) {
      auto& panelunit = Application::get().getHWUI()->getPanelUnit();
      auto& boled = panelunit.getEditPanel().getBoled();
      auto outPath = BankEditButtonMenu::createValidOutputPath(newExportName);
      boled.resetOverlay();
      boled.setOverlay(new SplashLayout());
      BankEditButtonMenu::writeSelectedBankToFile(bank, outPath);
      boled.resetOverlay();
    }));
  }
}

void BankEditButtonMenu::writeSelectedBankToFile(Bank* selBank, const std::string& outFile)
{
  GenericScopeGuard syncAfterAllFileOperation([] {}, FileSystem::syncAll);
  SplashLayout::addStatus("Exporting " + selBank->getName(true));
  auto scope = UNDO::Scope::startTrashTransaction();
  selBank->setAttribute(scope->getTransaction(), "Date of Export File", TimeTools::getAdjustedIso());
  selBank->setAttribute(scope->getTransaction(), "Name of Export File", outFile);
  PresetBankSerializer serializer(selBank, false);
  XmlWriter writer(std::make_unique<FileOutStream>(outFile, false));
  serializer.write(writer, VersionAttribute::get());
}

void BankEditButtonMenu::renameBank()
{
  auto layout = new RenameBankLayout();
  Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().setOverlay(layout);
}

void BankEditButtonMenu::copyBank()
{
  auto pm = Application::get().getPresetManager();

  if(auto bank = pm->getSelectedBank())
  {
    Application::get().getClipboard()->copyBank(bank->getUuid());
  }
}

void BankEditButtonMenu::pasteBank()
{
  auto pm = Application::get().getPresetManager();

  if(auto bank = pm->getSelectedBank())
  {
    Application::get().getClipboard()->pasteOnBank(bank->getUuid());
  }
}

void BankEditButtonMenu::deleteBank()
{
  auto pm = Application::get().getPresetManager();
  auto useCases = Application::get().getPresetManagerUseCases();
  if(auto bank = pm->getSelectedBank())
  {
    useCases->deleteBank(bank);
  }

  auto hwui = Application::get().getHWUI();
  hwui->setFocusAndMode(FocusAndMode(UIFocus::Banks, UIMode::Select));
  hwui->getPanelUnit().getEditPanel().getBoled().invalidate();
}

void BankEditButtonMenu::moveLeft()
{
  auto pm = Application::get().getPresetManager();

  if(auto bank = pm->getSelectedBank())
  {
    auto pos = pm->getBankPosition(bank->getUuid());
    if(pos > 0)
    {
      auto scope = bank->getUndoScope().startTransaction("Move bank '%0' left", bank->getName(true));
      pm->setOrderNumber(scope->getTransaction(), bank->getUuid(), pos - 1);
    }
  }
}

void BankEditButtonMenu::moveRight()
{
  auto pm = Application::get().getPresetManager();

  if(auto bank = pm->getSelectedBank())
  {
    auto pos = pm->getBankPosition(bank->getUuid()) + 1;
    if(pos < pm->getNumBanks())
    {
      auto scope = bank->getUndoScope().startTransaction("Move bank '%0' right", bank->getName(true));
      pm->setOrderNumber(scope->getTransaction(), bank->getUuid(), pos);
    }
  }
}

void BankEditButtonMenu::selectMidi()
{
  auto bank = Application::get().getPresetManager()->getSelectedBank();
  auto useCases = Application::get().getPresetManagerUseCases();
  useCases->selectMidiBank(bank);
}

void BankEditButtonMenu::removeMidi()
{
  auto useCases = Application::get().getPresetManagerUseCases();
  useCases->selectMidiBank(nullptr);
}

void BankEditButtonMenu::correctMenuSelection()
{
  if(s_lastSelectedButton >= getItemCount())
    s_lastSelectedButton = 0;
}

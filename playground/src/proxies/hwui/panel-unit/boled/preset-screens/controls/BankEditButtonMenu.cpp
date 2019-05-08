#include <Application.h>
#include <clipboard/Clipboard.h>
#include <libundo/undo/Scope.h>
#include <presets/PresetBank.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/BankEditButtonMenu.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/RenameBankLayout.h>
#include <tools/FileTools.h>
#include <serialization/PresetBankSerializer.h>
#include <xml/FileOutStream.h>
#include <proxies/hwui/panel-unit/boled/SplashLayout.h>
#include <proxies/hwui/panel-unit/boled/file/RenameExportLayout.h>
#include <proxies/hwui/panel-unit/boled/setup/USBStickAvailableView.h>
#include <proxies/hwui/panel-unit/boled/file/FileDialogLayout.h>
#include <device-info/DateTimeInfo.h>
#include <xml/VersionAttribute.h>
#include <proxies/hwui/panel-unit/boled/file/PleaseWaitLayout.h>
#include <tools/TimeTools.h>

static size_t s_lastSelectedButton = 0;

BankEditButtonMenu::BankEditButtonMenu(const Rect& rect)
    : super(rect)
{
  Application::get().getClipboard()->onClipboardChanged(mem_fun(this, &BankEditButtonMenu::rebuildMenu));
  Application::get().getPresetManager()->onNumBanksChanged(
      sigc::hide<0>(mem_fun(this, &BankEditButtonMenu::rebuildMenu)));
}

void BankEditButtonMenu::rebuildMenu()
{
  clear();
  clearActions();

  auto pm = Application::get().getPresetManager();
  if(pm->getNumBanks() != 0)
  {
    rebuildFullMenu();
  }
  else
  {
    rebuildNoBankAvailableMenu();
  }
  correctMenuSelection();
  selectButton(s_lastSelectedButton);
}

void BankEditButtonMenu::rebuildFullMenu()
{
  addButton("New", bind(&BankEditButtonMenu::newBank, this));

  if(USBStickAvailableView::usbIsReady())
  {
    addButton("Export ...", bind(&BankEditButtonMenu::exportBank, this));
    addButton("Import ...", bind(&BankEditButtonMenu::importBank, this));
  }

  addButton("Rename", bind(&BankEditButtonMenu::renameBank, this));
  addButton("Copy", bind(&BankEditButtonMenu::copyBank, this));

  if(Application::get().getClipboard()->hasContent())
  {
    addButton("Paste", bind(&BankEditButtonMenu::pasteBank, this));
  }

  addButton("Delete", bind(&BankEditButtonMenu::deleteBank, this));
  addButton("Move Left", bind(&BankEditButtonMenu::moveLeft, this));
  addButton("Move Right", bind(&BankEditButtonMenu::moveRight, this));
}

void BankEditButtonMenu::rebuildNoBankAvailableMenu()
{

  addButton("New", bind(&BankEditButtonMenu::newBank, this));

  if(USBStickAvailableView::usbIsReady())
  {
    addButton("Import ...", bind(&BankEditButtonMenu::importBank, this));
  }

  if(Application::get().getClipboard()->hasContent())
  {
    addButton("Paste", bind(&BankEditButtonMenu::pasteBank, this));
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
  auto newBank = pm->addBank(transaction, true);
  newBank->assignDefaultPosition();

  Application::get().getHWUI()->undoableSetFocusAndMode(transaction, FocusAndMode(UIFocus::Presets, UIMode::Select));

  auto layout = new RenameBankLayout(transaction);
  Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().setOverlay(layout);
}

BankEditButtonMenu::FileInfos BankEditButtonMenu::extractFileInfos(std::experimental::filesystem::directory_entry file)
{
  return FileInfos{ file };
}

bool BankEditButtonMenu::applicableBackupFilesFilter(std::experimental::filesystem::directory_entry term)
{
  auto fileName = term.path().filename().string();
  string end = ".xml";
  return !std::equal(end.rbegin(), end.rend(), fileName.rbegin());
}

void BankEditButtonMenu::importBankFromPath(std::experimental::filesystem::directory_entry file)
{
  auto hwui = Application::get().getHWUI();
  if(file != std::experimental::filesystem::directory_entry())
  {

    auto fileInfos = extractFileInfos(file);

    hwui->getPanelUnit().getEditPanel().getBoled().setOverlay(new SplashLayout());

    FileInStream stream(fileInfos.filePath, false);
    SplashLayout::addStatus("Importing " + fileInfos.fileName);
    Application::get().getPresetManager()->importBank(stream, "", "", fileInfos.fileName);
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
  auto fileName = FileTools::findSuitableFileName(bankName, "/mnt/usb-stick/", 0);
  return "/mnt/usb-stick/" + fileName + ".xml";
}

void BankEditButtonMenu::exportBank()
{
  auto& panelunit = Application::get().getHWUI()->getPanelUnit();
  auto& boled = panelunit.getEditPanel().getBoled();

  if(auto selBank = Application::get().getPresetManager()->getSelectedBank())
  {
    boled.setOverlay(new RenameExportLayout(selBank, [](Glib::ustring newExportName, std::shared_ptr<PresetBank> bank) {
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

void BankEditButtonMenu::writeSelectedBankToFile(PresetManager::tBankPtr selBank, const std::string& outFile)
{
  SplashLayout::addStatus("Exporting " + selBank->getName(true));
  selBank->setAttribute("Date of Export File", TimeTools::getAdjustedIso());
  selBank->setAttribute("Name of Export File", outFile);
  PresetBankSerializer serializer(selBank, false);
  XmlWriter writer(std::make_shared<FileOutStream>(outFile, false));
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

  if(auto bank = pm->getSelectedBank())
  {
    auto scope = bank->getUndoScope().startTransaction("Delete bank '%0'", bank->getName(true));
    pm->undoableDeleteSelectedBank(scope->getTransaction());
  }

  if(pm->getNumBanks() == 0)
  {
    auto hwui = Application::get().getHWUI();
    hwui->setFocusAndMode(FocusAndMode(UIFocus::Banks, UIMode::Select));
    hwui->getPanelUnit().getEditPanel().getBoled().invalidate();
  }

  rebuildMenu();
}

void BankEditButtonMenu::moveLeft()
{
  auto pm = Application::get().getPresetManager();

  if(auto bank = pm->getSelectedBank())
  {
    auto scope = bank->getUndoScope().startTransaction("Move bank '%0' left", bank->getName(true));
    pm->undoableChangeBankOrder(scope->getTransaction(), bank->getUuid(), PresetManager::moveDirection::LeftByOne);
  }
}

void BankEditButtonMenu::moveRight()
{
  auto pm = Application::get().getPresetManager();

  if(auto bank = pm->getSelectedBank())
  {
    auto scope = bank->getUndoScope().startTransaction("Move bank '%0' right", bank->getName(true));
    pm->undoableChangeBankOrder(scope->getTransaction(), bank->getUuid(), PresetManager::moveDirection::RightByOne);
  }
}

void BankEditButtonMenu::correctMenuSelection()
{
  if(s_lastSelectedButton >= getNumChildren())
    s_lastSelectedButton = 0;
}

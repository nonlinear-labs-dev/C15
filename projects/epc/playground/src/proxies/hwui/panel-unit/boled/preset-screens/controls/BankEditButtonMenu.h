#pragma once

#include "proxies/hwui/controls/ButtonMenu.h"
#include <filesystem>
#include <utility>

class Bank;

class BankEditButtonMenu : public ButtonMenu
{
 private:
  typedef ButtonMenu super;

 public:
  explicit BankEditButtonMenu(const Rect& rect);
  ~BankEditButtonMenu() override = default;

  void selectButton(size_t i) override;
  void rebuildMenu(size_t numBanks);
  void onMidiSelectionChanged(Uuid bankUuid);

 private:
  void rebuildMenuOnClipboardChange();
  void rebuildFullMenu();
  void rebuildNoBankAvailableMenu();

  void correctMenuSelection();

  BankEditButtonMenu(const BankEditButtonMenu& other);
  BankEditButtonMenu& operator=(const BankEditButtonMenu&);

  void newBank();
  void importBank();
  void renameBank();
  void exportBank();
  void copyBank();
  void pasteBank();
  void deleteBank();
  void moveLeft();
  void moveRight();
  void selectMidi();
  void removeMidi();

  static Glib::ustring createValidOutputPath(const Glib::ustring& bankName);

  static void writeSelectedBankToFile(Bank* selBank, const std::string& outFile);
  static void importBankFromPath(const std::filesystem::directory_entry& file);
  static bool applicableBackupFilesFilter(const std::filesystem::directory_entry& term);
};

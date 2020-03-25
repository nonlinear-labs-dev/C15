#pragma once

#include "proxies/hwui/controls/ButtonMenu.h"
#include <experimental/filesystem>
#include <utility>

class Bank;

class BankEditButtonMenu : public ButtonMenu
{
 private:
  typedef ButtonMenu super;

  struct FileInfos
  {
    explicit FileInfos(const std::experimental::filesystem::directory_entry& file)
    {
      filePath = file.path().string();
      fileName = file.path().filename().string();
      auto lastModified = std::experimental::filesystem::last_write_time(file);
      millisecondsFromEpoch
          = std::chrono::duration_cast<std::chrono::milliseconds>(lastModified.time_since_epoch()).count();
    }

    FileInfos(std::string name, std::string path, long stamp)
        : fileName { std::move(name) }
        , filePath { std::move(path) }
        , millisecondsFromEpoch { stamp }
    {
    }

    std::string fileName;
    std::string filePath;
    long millisecondsFromEpoch;
  };

 public:
  explicit BankEditButtonMenu(const Rect& rect);
  ~BankEditButtonMenu() override = default;

  void selectButton(size_t i) override;
  void rebuildMenu(size_t numBanks);

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

  static Glib::ustring createValidOutputPath(const Glib::ustring& bankName);

  static void writeSelectedBankToFile(Bank* selBank, const std::string& outFile);
  static FileInfos extractFileInfos(const std::experimental::filesystem::directory_entry& file);
  static void importBankFromPath(const std::experimental::filesystem::directory_entry& file);
  static bool applicableBackupFilesFilter(const std::experimental::filesystem::directory_entry& term);
};

#pragma once

#include "proxies/hwui/controls/ButtonMenu.h"
#include <experimental/filesystem>

class PresetBank;

class BankEditButtonMenu : public ButtonMenu
{
 private:
  typedef ButtonMenu super;

  struct FileInfos
  {
    FileInfos(std::experimental::filesystem::directory_entry file)
    {
      filePath = file.path().string();
      fileName = file.path().filename().string();
      auto lastModified = std::experimental::filesystem::last_write_time(file);
      millisecondsFromEpoch
          = std::chrono::duration_cast<std::chrono::milliseconds>(lastModified.time_since_epoch()).count();
    }
    FileInfos(std::string name, std::string path, long stamp)
        : fileName{ name }
        , filePath{ path }
        , millisecondsFromEpoch{ stamp }
    {
    }
    std::string fileName;
    std::string filePath;
    long millisecondsFromEpoch;
  };

 public:
  explicit BankEditButtonMenu(const Rect& rect);
  virtual ~BankEditButtonMenu() = default;

  void selectButton(size_t i) override;
  void rebuildMenu();

 private:
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

  static void writeSelectedBankToFile(PresetManager::tBankPtr selBank, const std::string& outFile);
  static FileInfos extractFileInfos(std::experimental::filesystem::directory_entry file);
  static void importBankFromPath(std::experimental::filesystem::directory_entry file);
  static bool applicableBackupFilesFilter(std::experimental::filesystem::directory_entry term);
};

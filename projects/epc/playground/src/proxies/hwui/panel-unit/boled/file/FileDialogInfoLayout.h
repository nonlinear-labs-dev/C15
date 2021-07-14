#pragma once
#include <proxies/hwui/panel-unit/boled/parameter-screens/InfoLayout.h>
#include <proxies/hwui/panel-unit/boled/file/FileDialogLayout.h>
#include <filesystem>

class FileDialogInfoLayout : public InfoLayout
{
 public:
  FileDialogInfoLayout(std::filesystem::directory_entry file, std::string header);
  ~FileDialogInfoLayout() override = default;

  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;

  void addModuleCaption() override;
  void addHeadline() override;
  void addInfoLabel() override;
  Scrollable* createScrollableContent() override;

 private:
  std::filesystem::directory_entry m_file;
  std::string m_header;
};

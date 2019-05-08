#pragma once
#include <proxies/hwui/panel-unit/boled/parameter-screens/InfoLayout.h>
#include <proxies/hwui/panel-unit/boled/file/FileDialogLayout.h>
#include <experimental/filesystem>

class FileDialogInfoLayout : public InfoLayout
{
 public:
  FileDialogInfoLayout(std::experimental::filesystem::directory_entry file, std::string header);
  virtual ~FileDialogInfoLayout() = default;

  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;

  void addModuleCaption() override;
  void addHeadline() override;
  void addInfoLabel() override;
  Scrollable* createScrollableContent() override;

 private:
  std::experimental::filesystem::directory_entry m_file;
  std::string m_header;
};

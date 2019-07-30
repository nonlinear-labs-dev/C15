#pragma once

#include <proxies/hwui/DFBLayout.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/FileListControl.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/InvertedLabel.h>
#include <tools/FileTools.h>
#include <Application.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/controls/Label.h>
#include <experimental/filesystem>
#include <tools/BackgroundJobs.h>

class FileDialogLayout : public DFBLayout
{
 private:
  typedef std::function<void(std::experimental::filesystem::directory_entry)> tCallBackFunction;
  typedef std::function<bool(std::experimental::filesystem::directory_entry)> tFilterFunction;

 public:
  FileDialogLayout(tFilterFunction filter, tCallBackFunction cb, const std::string& header);
  ~FileDialogLayout() override;

  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
  bool onRotary(int inc, ButtonModifiers modifiers) override;

  std::experimental::filesystem::directory_entry getSelectedFile();

 private:
  void updateLabels();
  void overlayInfo();

  int fileCount = 0;
  FileListControl* fileList = nullptr;
  Label* positionLabel = nullptr;
  InvertedLabel* headerLabel = nullptr;
  tCallBackFunction commitFunction;
  FileCrawlerJob crawler;

  std::string m_header;
};

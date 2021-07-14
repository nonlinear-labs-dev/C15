#pragma once

#include <proxies/hwui/controls/ControlWithChildren.h>
#include <tools/FileSystem.h>
#include <filesystem>

namespace fs = std::filesystem;

class Label;

class FileListControl : public ControlWithChildren
{
 public:
  explicit FileListControl(const Rect& pos);
  ~FileListControl() override;

  void setFileList(FileSystem::FileList list);
  void setSelection(int index);
  void changeSelection(int change);
  fs::directory_entry getSelection() const;
  int getSelectedIndex() const;

 private:
  std::string getFileNameAtIndex(int index);

  Label* upperFile;
  Label* middleFile;
  Label* bottomFile;
  std::unique_ptr<FileSystem::FileList> fileList;
  int selectedItem;
};

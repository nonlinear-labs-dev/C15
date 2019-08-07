
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/FileListControl.h>

FileListControl::FileListControl(const Rect &pos)
    : selectedItem(0)
    , ControlWithChildren(pos)
{
  fileList = std::make_unique<FileTools::FileList>();
  upperFile = addControl(new Label("", Rect(0, 0, 256, 12)));
  middleFile = addControl(new Label("", Rect(0, 12, 256, 12)));
  bottomFile = addControl(new Label("", Rect(0, 24, 256, 12)));

  middleFile->setHighlight(true);

  setSelection(0);
}

void FileListControl::setFileList(FileTools::FileList list)
{
  fileList = std::make_unique<FileTools::FileList>(list);
  setSelection(selectedItem);
}

FileListControl::~FileListControl() = default;

void FileListControl::setSelection(int index)
{
  upperFile->setText({ getFileNameAtIndex(index - 1) });
  middleFile->setText({ getFileNameAtIndex(index) });
  bottomFile->setText({ getFileNameAtIndex(index + 1) });

  selectedItem = index;
}

std::string FileListControl::getFileNameAtIndex(int index)
{
  if(index >= 0 && index < fileList->size())
    return fileList->at(index).path().filename().string();
  return "";
}

void FileListControl::changeSelection(int change)
{
  auto newIndex = selectedItem + change;

  if(newIndex < 0)
    setSelection(0);
  else if(newIndex >= fileList->size())
    setSelection((int) fileList->size() - 1);
  else
    setSelection(newIndex);
}

fs::directory_entry FileListControl::getSelection() const
{
  if(selectedItem < fileList->size())
    return fileList->at(selectedItem);
  else
    return fs::directory_entry();
}

int FileListControl::getSelectedIndex() const
{
  return selectedItem;
}

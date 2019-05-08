#pragma once

#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/Scrollable.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MultiLineLabel.h>
#include <experimental/filesystem>
#include <proxies/hwui/panel-unit/boled/InfoContent.h>

class FileInfoContent : public InfoContent
{
 public:
  FileInfoContent(std::experimental::filesystem::directory_entry file);
  ~FileInfoContent() = default;

  void setPosition(const Rect &rect) override;
  const Rect &getPosition() const override;
  void setDirty() override;

  void fixLayout();

 private:
  void initializeFileInfosFromFile();
  std::experimental::filesystem::directory_entry m_file;
};

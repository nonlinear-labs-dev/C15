#pragma once

#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/Scrollable.h>
#include <filesystem>
#include <proxies/hwui/panel-unit/boled/info/InfoContent.h>

class FileInfoContent : public InfoContent
{
 public:
  explicit FileInfoContent(std::filesystem::directory_entry file);

  void fillContents() override;

 private:
  std::filesystem::directory_entry m_file;
};

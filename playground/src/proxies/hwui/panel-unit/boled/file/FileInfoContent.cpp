#include <proxies/hwui/panel-unit/boled/file/FileInfoContent.h>
#include <proxies/hwui/controls/LeftAlignedLabel.h>
#include <regex>
#include <chrono>
#include <utility>
#include <tools/FileTools.h>

FileInfoContent::FileInfoContent(std::experimental::filesystem::directory_entry file)
    : m_file(std::move(file))
{
  initializeFileInfosFromFile();
}

std::string writeTimeToString(std::experimental::filesystem::file_time_type ftime)
{
  std::time_t cftime = decltype(ftime)::clock::to_time_t(ftime);
  std::string stringWithTrailingNewLine(std::asctime(std::localtime(&cftime)));
  return stringWithTrailingNewLine.substr(0, stringWithTrailingNewLine.size() - 1);
}

void FileInfoContent::initializeFileInfosFromFile()
{
  addInfoField("filename", "Filename:");
  addInfoField("path", "Path:", new MultiLineContent());
  addInfoField("date", "Mod. Date:");
  addInfoField("size", "File Size:");

  const auto filePath = std::regex_replace(m_file.path().parent_path().string(), std::regex("/mnt/usb-stick"), "");

  infoFields["filename"]->setInfo(m_file.path().filename().string());
  infoFields["path"]->setInfo(filePath.empty() ? "/" : to_string(filePath));
  infoFields["date"]->setInfo(writeTimeToString(std::experimental::filesystem::last_write_time(m_file.path())));
  infoFields["size"]->setInfo(FileTools::bytesToHumanreadable(std::experimental::filesystem::file_size(m_file.path())));

  fixLayout();
}

void FileInfoContent::setPosition(const Rect &rect)
{
  InfoContent::setPosition(rect);
  fixLayout();
}

const Rect &FileInfoContent::getPosition() const
{
  return InfoContent::getPosition();
}

void FileInfoContent::setDirty()
{
  InfoContent::setDirty();
}

void FileInfoContent::fixLayout()
{
  int y = 0;

  for(const auto& key : { "filename", "path", "date", "size" })
  {
    y = infoFields[key]->format(y);
  }

  Rect r = getPosition();
  r.setHeight(y);
  InfoContent::setPosition(r);
}

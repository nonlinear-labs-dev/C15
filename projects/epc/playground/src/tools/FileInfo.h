#pragma once
#include <filesystem>
#include <string>
#include <chrono>

struct FileInfos
{
  explicit FileInfos(const std::filesystem::directory_entry& file)
  {
    filePath = file.path().string();
    fileName = file.path().filename().string();
    auto lastModified = std::filesystem::last_write_time(file);
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
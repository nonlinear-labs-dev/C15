#pragma once

#include <utility>
#include <vector>
#include <experimental/filesystem>
#include <nltools/system/SpawnCommandLine.h>
#include <device-settings/DebugLevel.h>
#include <fstream>
#include <giomm/file.h>

namespace FileTools
{
  namespace fs = std::experimental::filesystem;

  typedef std::vector<fs::directory_entry> FileList;

  static void rename(const Glib::ustring& oldFile, const Glib::ustring& newFile)
  {
    SpawnCommandLine cmd(Glib::ustring::format("mv ", oldFile, " ", newFile));
  }

  static Glib::ustring findSuitableFileName(const Glib::ustring& desiredName, const Glib::ustring& directoryPath,
                                            int depth)
  {
    auto fileName = desiredName;
    if(depth != 0)
      fileName.append(std::to_string(depth));

    for(const auto& p : fs::directory_iterator(directoryPath.c_str()))
    {
      if(p.path().filename().stem().string() == fileName)
      {
        depth++;
        return findSuitableFileName(desiredName, directoryPath, depth);
      }
    }
    return fileName;
  }

  static bool doesFileExists(const Glib::ustring& absolutePath)
  {
    const auto filePath = fs::path(absolutePath.c_str());
    return fs::exists(filePath);
  }

  static bool writeToFile(const Glib::ustring& path, const char* content)
  {
    if(fs::exists(fs::path(path.c_str())))
    {
      std::ofstream outStream(path, std::ios::binary);
      outStream << content;
      outStream.close();
      return not outStream.fail();
    }
    return false;
  }

  static Glib::ustring readFromFile(const Glib::ustring& path)
  {
    if(fs::exists(fs::path(path.c_str())))
    {
      Glib::ustring ret;
      std::ifstream inStream(path, std::ios::binary);
      inStream >> ret;
      inStream.close();
      return ret;
    }
    return "";
  }

  static Glib::ustring bytesToHumanreadable(size_t numBytes)
  {
    gchar* stringPtr = g_format_size(numBytes);
    Glib::ustring ret(stringPtr);
    g_free(stringPtr);
    return ret;
  }

  static Glib::ustring getFullPath(const Glib::RefPtr<Gio::File>& file)
  {
    return file->get_path();
  }
};

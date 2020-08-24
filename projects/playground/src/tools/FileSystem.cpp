#include <tools/FileSystem.h>
#include <filesystem>
#include <fstream>
#include <giomm.h>
#include <nltools/logging/Log.h>
#include <nltools/messaging/Messaging.h>
#include <nltools/messaging/Message.h>
#include <thread>

void FileSystem::deleteFolder(Glib::RefPtr<Gio::File> folder)
{
  Glib::RefPtr<Gio::FileEnumerator> enumerator = folder->enumerate_children();
  std::list<Glib::RefPtr<Gio::FileInfo>> toDelete;

  while(auto file = enumerator->next_file())
  {
    auto type = file->get_file_type();

    if(type == Gio::FILE_TYPE_SYMBOLIC_LINK || type == Gio::FILE_TYPE_REGULAR)
    {
      auto name = file->get_name();
      auto file = folder->get_child(name);
      file->remove();
    }
    else if(type == Gio::FILE_TYPE_DIRECTORY)
    {
      auto name = file->get_name();
      auto file = folder->get_child(name);
      deleteFolder(file);
      file->remove();
    }
  }
  folder->remove();
}

bool FileSystem::isNameAUUID(const Glib::ustring &name)
{
  auto fileName = name.substr(0, name.find("."));
  return fileName.size() == 36;
}

void FileSystem::rename(const Glib::ustring &oldFile, const Glib::ustring &newFile)
{
  using namespace std::chrono_literals;

  std::error_code ec;
  std::filesystem::path oldName(oldFile);
  std::filesystem::path newName(newFile);
  std::filesystem::rename(oldName, newName, ec);

  if(ec)
  {
    nltools::Log::warning("Could not rename file", oldFile, "into", newFile, ":", ec.message());
    return;
  }

  // sshfs file system seems to need some time to rename the file
  auto waitUntil = std::chrono::steady_clock::now() + 10s;

  while(!std::filesystem::exists(newName) && std::chrono::steady_clock::now() < waitUntil)
  {
    std::this_thread::sleep_for(250ms);
    nltools::Log::info("Waiting for file to be actually renamed...");
  }

  if(!std::filesystem::exists(newName))
    nltools::Log::error("Renamed file does still not exist.");
  else
    nltools::Log::info("Successfully renamed file", oldFile, "into", newFile);
}

Glib::ustring FileSystem::findSuitableFileName(const Glib::ustring &desiredName, const Glib::ustring &directoryPath,
                                               int depth)
{
  auto fileName = desiredName;
  if(depth != 0)
    fileName.append(std::to_string(depth));

  for(const auto &p : std::filesystem::directory_iterator(directoryPath.c_str()))
  {
    if(p.path().filename().stem().string() == fileName)
    {
      depth++;
      return findSuitableFileName(desiredName, directoryPath, depth);
    }
  }
  return fileName;
}

bool FileSystem::doesFileExist(const Glib::ustring &absolutePath)
{
  const auto filePath = std::filesystem::path(absolutePath.c_str());
  return std::filesystem::exists(filePath);
}

bool FileSystem::writeToFile(const Glib::ustring &path, const char *content)
{
  if(std::filesystem::exists(std::filesystem::path(path.c_str())))
  {
    std::ofstream outStream(path, std::ios::binary);
    outStream << content;
    outStream.close();
    return not outStream.fail();
  }
  return false;
}

Glib::ustring FileSystem::readFromFile(const Glib::ustring &path)
{
  if(std::filesystem::exists(std::filesystem::path(path.c_str())))
  {
    Glib::ustring ret;
    std::ifstream inStream(path, std::ios::binary);
    inStream >> ret;
    inStream.close();
    return ret;
  }
  return "";
}

Glib::ustring FileSystem::bytesToHumanreadable(size_t numBytes)
{
  gchar *stringPtr = g_format_size(numBytes);
  Glib::ustring ret(stringPtr);
  g_free(stringPtr);
  return ret;
}

Glib::ustring FileSystem::getFullPath(const Glib::RefPtr<Gio::File> &file)
{
  return file->get_path();
}

void FileSystem::syncAll()
{
  nltools::Log::info("Syncing file systems ...");
  sync();
  nltools::msg::send(nltools::msg::EndPoint::BeagleBone, nltools::msg::FileSystem::Sync());
  nltools::Log::info("Syncing file systems done.");
}

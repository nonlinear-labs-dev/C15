#include <utility>

#include "RecursiveDirectoryMonitor.h"

namespace FileTools
{
  RecursiveDirectoryMonitor::RecursiveDirectoryMonitor(const tFile& rootFolder, tCallBack callback)
      : m_callBack(std::move(callback))
      , m_rootFolder(rootFolder)
      , m_monitors(0)
  {
    rebuildDirectoryList();
  }

  void RecursiveDirectoryMonitor::rebuildDirectoryList()
  {
    m_monitors.clear();
    addMonitor(m_rootFolder);
    auto list = getAllDirectorysInDirectory(m_rootFolder);
    for(auto& dir : list)
    {
      addMonitor(dir);
    }
  }

  void RecursiveDirectoryMonitor::addMonitor(const tFile& file)
  {
    auto monitor = file->monitor(Gio::FILE_MONITOR_WATCH_MOUNTS);
    monitor->signal_changed().connect(sigc::mem_fun(this, &RecursiveDirectoryMonitor::onFileChanged));
    m_monitors.emplace_back(std::move(monitor));
  }

  void RecursiveDirectoryMonitor::onFileChanged(const tFile& oldFile, const tFile& newFile,
                                                Gio::FileMonitorEvent monitorEvent)
  {
    m_callBack(oldFile, newFile, monitorEvent);
    rebuildDirectoryList();
  }

  void RecursiveDirectoryMonitor::recurseDirectory(const tFile& start, tFileCallBack cb)
  {
    auto fileIt = start->enumerate_children();
    while(auto fileInfo = fileIt->next_file())
    {
      auto file = getFileFromFileInfo(start, fileInfo);
      cb(file);
      if(fileInfo->get_file_type() == Gio::FILE_TYPE_DIRECTORY)
        recurseDirectory(file, cb);
    }
  }

  RecursiveDirectoryMonitor::tFileList RecursiveDirectoryMonitor::getAllDirectorysInDirectory(const tFile& folder)
  {
    tFileList ret;
    recurseDirectory(folder, [&](const tFile& file) {
      if(file->query_info()->get_file_type() == Gio::FILE_TYPE_DIRECTORY)
        ret.push_back(file);
    });
    return ret;
  }

  RecursiveDirectoryMonitor::tFileList RecursiveDirectoryMonitor::getAllFilesInFolder(const tFile& folder)
  {
    tFileList ret;
    recurseDirectory(folder, [&](const tFile& file) {
      if(file->query_info()->get_file_type() == Gio::FILE_TYPE_REGULAR)
        ret.push_back(file);
    });
    return ret;
  }

  RecursiveDirectoryMonitor::tFile
      RecursiveDirectoryMonitor::getFileFromFileInfo(const tFile& currentFolder,
                                                     const Glib::RefPtr<Gio::FileInfo>& fileInfo)
  {
    auto name = fileInfo->get_name();
    auto path = currentFolder->get_path() + '/' + name;
    return Gio::File::create_for_path(path);
  }
}

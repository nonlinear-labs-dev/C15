#pragma once

#include <functional>
#include <thread>
#include <assert.h>
#include "FileTools.h"

class BackgroundJob
{
 protected:
  typedef std::function<void(void)> tCallback;

 public:
  BackgroundJob(tCallback cb)
      : callback(cb)
  {
  }

  virtual void start()
  {
    m_thread = std::thread([&]() {
      while(iterate() && !m_close)
      {
        callback();
      }
    });
  }

  virtual bool iterate() = 0;

  void killMe()
  {
    m_close = true;
    if(m_thread.joinable())
      m_thread.join();
  }

  ~BackgroundJob()
  {
    assert(m_close == true);
  }

  bool isRunning()
  {
    return !m_close;
  }

 protected:
  std::thread m_thread;
  tCallback callback;
  bool m_close = false;
};

namespace fs = std::experimental::filesystem;

class FileCrawlerJob : public BackgroundJob
{
 protected:
  typedef std::function<bool(fs::directory_entry)> tFilterFunction;

 public:
  FileCrawlerJob(std::string dir, tFilterFunction filter, tCallback cb)
      : BackgroundJob(cb)
      , fileFilter(filter)
  {
    it = std::experimental::filesystem::recursive_directory_iterator(fs::path(dir.c_str()));
  }

  bool iterate() override
  {
    if(it != fs::recursive_directory_iterator())
    {
      try
      {
        Glib::ustring name(it->path().string());
        if(fs::is_directory(it->path()) && name.find("/.") != Glib::ustring::npos)
        {
          it.disable_recursion_pending();
        }
        else if(!fs::is_directory(it->path()) && !fileFilter(*it))
        {
          std::lock_guard<std::mutex> lock(m);
          list.emplace_back(*it);
        }
      }
      catch(...)
      {
        DebugLevel::error("Error while traversing Thumbdrive!");
      }
      ++it;
      return true;
    }
    else
    {
      return false;
    }
  }

  FileTools::FileList copyData()
  {
    std::lock_guard<std::mutex> lock(m);
    return list;
  }

 protected:
  std::mutex m;
  FileTools::FileList list;
  std::function<bool(fs::directory_entry)> fileFilter;
  fs::recursive_directory_iterator it;
};

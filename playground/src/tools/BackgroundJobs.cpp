#include <utility>

#include "BackgroundJobs.h"

BackgroundJob::BackgroundJob(BackgroundJob::tCallback cb)
    : callback(std::move(cb))
{
}

BackgroundJob::~BackgroundJob()
{
  assert(m_close);
}

void BackgroundJob::start()
{
  m_thread = std::thread([&]() {
    while(iterate() && !m_close)
    {
      callback();
    }
  });
}

void BackgroundJob::killMe()
{
  m_close = true;
  if(m_thread.joinable())
    m_thread.join();
}

bool BackgroundJob::isRunning()
{
  return !m_close;
}

FileCrawlerJob::FileCrawlerJob(std::string dir, FileCrawlerJob::tFilterFunction filter, BackgroundJob::tCallback cb)
    : BackgroundJob(std::move(cb))
    , fileFilter(std::move(filter))
{
  it = std::experimental::filesystem::recursive_directory_iterator(fs::path(dir.c_str()));
}

bool FileCrawlerJob::iterate()
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

FileTools::FileList FileCrawlerJob::copyData()
{
  std::lock_guard<std::mutex> lock(m);
  return list;
}

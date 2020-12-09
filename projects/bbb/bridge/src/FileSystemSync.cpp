#include "FileSystemSync.h"
#include <nltools/messaging/Messaging.h>
#include <nltools/messaging/Message.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

FileSystemSync::FileSystemSync()
    : m_task(std::async(std::launch::async, [this] { doBackgroundSyncing(); }))
{
  nltools::msg::receive<nltools::msg::FileSystem::Sync>(nltools::msg::EndPoint::BeagleBone, [this](const auto &) {
    nltools::Log::warning("Notify syncing file system ...");
    std::unique_lock<std::mutex> l(m_mutex);
    m_dirty = true;
    m_cond.notify_all();
    nltools::Log::warning("Notify syncing file system done.");
  });
}

FileSystemSync::~FileSystemSync()
{
  {
    std::unique_lock<std::mutex> l(m_mutex);
    m_cancel = true;
  }
  m_cond.notify_all();
  m_task.wait();
}

void FileSystemSync::doBackgroundSyncing()
{
  pthread_setname_np(pthread_self(), "FSSync");

  std::unique_lock<std::mutex> l(m_mutex);

  while(!m_cancel)
  {
    if(!m_dirty)
      m_cond.wait(l, [this] { return m_dirty || m_cancel; });

    m_dirty = false;
    l.unlock();
    nltools::Log::warning("Syncing file system ...");
    sync();
    nltools::Log::warning("Syncing file system done.");
    l.lock();
  }
}

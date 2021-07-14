#include <utility>

#include <glibmm/spawn.h>
#include <glibmm/main.h>
#include <nltools/Assert.h>
#include <glibmm/shell.h>
#include <unistd.h>
#include "nltools/system/AsyncCommandLine.h"

AsyncCommandLine::AsyncCommandLine(const std::vector<std::string>& command,
                                   std::function<void(const std::string&)> onSuccess,
                                   std::function<void(const std::string&)> onError)
    : m_onError { std::move(onError) }
    , m_onSuccess { std::move(onSuccess) }
    , m_childPid { 0 }
{
  Glib::spawn_async_with_pipes("", command, Glib::SPAWN_DO_NOT_REAP_CHILD | Glib::SPAWN_SEARCH_PATH,
                               Glib::SlotSpawnChildSetup(), &m_childPid, nullptr, &m_coutFD, &m_cerrFD);

  nltools_detailedAssertAlways(m_childPid != 0, "Child PID can not be 0");

  m_streamCout = Glib::IOChannel::create_from_fd(m_coutFD);
  m_streamCerr = Glib::IOChannel::create_from_fd(m_cerrFD);
  m_isRunning = true;

  Glib::signal_child_watch().connect(sigc::mem_fun(*this, &AsyncCommandLine::watchHandler), m_childPid);
}

AsyncCommandLine::~AsyncCommandLine()
{
  closeWatch(m_childPid);
  close(m_coutFD);
  close(m_cerrFD);
}

void AsyncCommandLine::watchHandler(Glib::Pid pid, int status)
{
  nltools_detailedAssertAlways(pid == m_childPid, "AsyncCommandLine: Child PID changed since launch");

  Glib::ustring cout {}, cerr {};

  m_streamCout->read_to_end(cout);
  m_streamCerr->read_to_end(cerr);

  closeWatch(pid);

  if(status == 0)
  {
    m_onSuccess(cout);
  }
  else
  {
    m_onError(cerr);
  }
}
void AsyncCommandLine::closeWatch(Glib::Pid pid)
{
  Glib::spawn_close_pid(pid);
  m_isRunning = false;
}

bool AsyncCommandLine::isRunning() const
{
  return m_isRunning;
}

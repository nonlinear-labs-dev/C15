#include "HWTest.h"
#include "Application.h"
#include "Options.h"

HWTest::HWTest(RefPtr<Gio::FileInfo> fileInfo)
    : m_fileInfo(fileInfo)
    , m_state(FRESH)
    , m_watchHandle(0)
{
}

HWTest::~HWTest()
{
  if(m_watchHandle)
    g_source_remove(m_watchHandle);
}

HWTest::State HWTest::getState() const
{
  return m_state;
}

Glib::ustring HWTest::getOutput() const
{
  return m_output;
}

void HWTest::watch(GPid pid, gint status, HWTest *pThis)
{
  pThis->m_state = (status == EXIT_SUCCESS) ? SUCCESS : FAILED;
  pThis->m_child_pid = 0;
  g_source_remove(pThis->m_watchHandle);
  pThis->m_watchHandle = 0;
  pThis->m_statusChanged.send();
}

void HWTest::onTestChanged(sigc::slot<void> s)
{
  m_statusChanged.connectAndInit(s);
}

void HWTest::execute()
{
  if(m_state == RUNNING)
  {
    kill(m_child_pid, SIGQUIT);
  }
  else
  {
    m_state = RUNNING;
    m_output.clear();

    GError *error = NULL;
    gchar *argv[2];
    argv[0] = g_strdup(getFileName().c_str());
    argv[1] = NULL;

    gint stdIn = 0;
    gint stdOut = 0;
    gint stdErr = 0;

    g_spawn_async_with_pipes(Application::get().getOptions()->getHardwareTestsFolder().c_str(), argv, NULL,
                             (GSpawnFlags) 0, NULL, NULL, &m_child_pid, &stdIn, &stdOut, &stdErr, &error);

    if(error)
    {
      m_output = error->message;
      m_state = FAILED;
      g_error_free(error);
    }
    else
    {
      m_inputStream = Gio::UnixOutputStream::create(stdIn, TRUE);
      m_outputStream = Gio::UnixInputStream::create(stdOut, TRUE);
      m_errorStream = Gio::UnixInputStream::create(stdErr, TRUE);
      m_watchHandle = g_child_watch_add(m_child_pid, (GChildWatchFunc) &HWTest::watch, this);

      read();
    }

    m_statusChanged.send();

    g_free(argv[0]);
  }
}

void HWTest::read()
{
  m_outputStream->read_bytes_async(1024, mem_fun(this, &HWTest::onOutputRead));
}

void HWTest::onOutputRead(RefPtr<Gio::AsyncResult> &result)
{
  Glib::RefPtr<Glib::Bytes> res = m_outputStream->read_bytes_finish(result);

  if(res->get_size())
  {
    gsize len = 0;
    const char *ptr = (const char *) res->get_data(len);
    m_output += Glib::ustring(ptr, ptr + len);
    m_statusChanged.send();
    read();
  }
}

Glib::ustring HWTest::getName() const
{
  return m_fileInfo->get_display_name();
}

Glib::ustring HWTest::getFileName() const
{
  return m_fileInfo->get_name();
}

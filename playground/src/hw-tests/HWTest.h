#pragma once

#include "playground.h"
#include <giomm/file.h>

class Application;

class HWTest
{
 public:
  HWTest(RefPtr<Gio::FileInfo> fileInfo);
  virtual ~HWTest();

  void execute();
  Glib::ustring getName() const;
  Glib::ustring getFileName() const;

  enum State
  {
    FRESH,
    RUNNING,
    SUCCESS,
    FAILED
  };

  State getState() const;
  Glib::ustring getOutput() const;

  void onTestChanged(sigc::slot<void> s);

 private:
  static void watch(GPid pid, gint status, HWTest* pThis);
  void read();
  void onOutputRead(RefPtr<Gio::AsyncResult>& result);

  Signal<void> m_statusChanged;
  RefPtr<Gio::FileInfo> m_fileInfo;
  State m_state;
  Glib::ustring m_output;

  Glib::RefPtr<Gio::UnixOutputStream> m_inputStream;
  Glib::RefPtr<Gio::UnixInputStream> m_outputStream;
  Glib::RefPtr<Gio::UnixInputStream> m_errorStream;
  GPid m_child_pid;
  guint m_watchHandle;
};

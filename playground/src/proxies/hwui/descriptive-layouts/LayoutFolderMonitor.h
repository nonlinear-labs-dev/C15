#pragma once

#include <giomm/file.h>
#include <tools/Signal.h>
#include <tools/RecursiveDirectoryMonitor.h>

class LayoutFolderMonitor : public sigc::trackable
{
 public:
  static LayoutFolderMonitor &get();

  void bruteForce();
  sigc::connection onChange(std::function<void()> cb);

 private:
  LayoutFolderMonitor();

  void onFileChanged(const Glib::RefPtr<Gio::File> &, const Glib::RefPtr<Gio::File> &, Gio::FileMonitorEvent);
  Signal<void> m_onChange;
  Glib::RefPtr<Gio::File> m_rootFolder;
  FileTools::RecursiveDirectoryMonitor m_recMonitor;
};

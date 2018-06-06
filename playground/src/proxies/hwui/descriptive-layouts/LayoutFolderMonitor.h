#pragma once

#include <giomm/file.h>
#include <tools/Signal.h>

class LayoutFolderMonitor : public sigc::trackable
{
  public:
    static LayoutFolderMonitor &get();
    virtual ~LayoutFolderMonitor();

    void bruteForce();
    sigc::connection onChange(std::function<void ()> cb);

  private:
    LayoutFolderMonitor();

    void onFileChanged(const Glib::RefPtr<Gio::File>&,const Glib::RefPtr<Gio::File>&, Gio::FileMonitorEvent);

    Glib::RefPtr<Gio::File> m_file;
    Glib::RefPtr<Gio::FileMonitor> m_monitor;
    Signal<void> m_onChange;
};


#include "LayoutFolderMonitor.h"
#include "LayoutFactory.h"
#include <Application.h>
#include <Options.h>
#include <proxies/hwui/debug-oled/DebugLayout.h>
#include <tools/json.h>
#include <execinfo.h>
#include <tools/ExceptionTools.h>
#include "LayoutParser.h"
#include "ControlRegistry.h"
#include "ControlParser.h"
#include "Styles.h"
#include "StyleParser.h"
#include "tools/SpawnCommandLine.h"

LayoutFolderMonitor& LayoutFolderMonitor::get()
{
  static LayoutFolderMonitor mon;
  return mon;
}

LayoutFolderMonitor::LayoutFolderMonitor()
{
  auto folder = Application::get().getOptions()->getLayoutFolder();
  m_file = Gio::File::create_for_path(folder);
  m_monitor = m_file->monitor(Gio::FILE_MONITOR_WATCH_MOUNTS);
  m_monitor->signal_changed().connect(sigc::mem_fun(this, &LayoutFolderMonitor::onFileChanged));
  bruteForce();
}

LayoutFolderMonitor::~LayoutFolderMonitor()
{
}

void LayoutFolderMonitor::onFileChanged(const Glib::RefPtr<Gio::File>&, const Glib::RefPtr<Gio::File>&, Gio::FileMonitorEvent)
{
  bruteForce();
}

void LayoutFolderMonitor::bruteForce()
{
  DescriptiveLayouts::BoledLayoutFactory::get().clear();
  DescriptiveLayouts::ControlRegistry::get().clear();
  DescriptiveLayouts::StyleSheet::get().clear();

  auto enumerator = m_file->enumerate_children();
  try {

    while (auto file = enumerator->next_file()) {
      auto name = file->get_name();
      auto path = m_file->get_path() + '/' + name;

      if (g_str_has_suffix(name.c_str(), ".json")) {
        DescriptiveLayouts::importControls(path);
        DescriptiveLayouts::importLayout(path);
        DescriptiveLayouts::importStyles(path);
      } else if (g_str_has_suffix(name.c_str(), ".yaml")) {
        auto tmpPath = "/tmp/__nl_style.json";
        SpawnCommandLine cmd("yaml2json " + path);
        g_file_set_contents(tmpPath, cmd.getStdOutput().c_str(), -1, nullptr);
        DescriptiveLayouts::importControls(tmpPath);
        DescriptiveLayouts::importLayout(tmpPath);
        DescriptiveLayouts::importStyles(tmpPath);
      }
    }

    m_onChange.send();

  }
  catch(ExceptionTools::TemplateException& e) {
    Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().reset(new DebugLayout(e.what() + e.where()));
  }
  catch (nlohmann::json::out_of_range& e)
  {
    Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().reset(new DebugLayout(e.what()));
  }
  catch (std::out_of_range &e) {
    Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().reset(new DebugLayout(e.what()));
  }
  catch(nlohmann::json::parse_error& e) {
    Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().reset(new DebugLayout(e.what()));
  }
  catch(std::runtime_error& e) {
    Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().reset(new DebugLayout(e.what()));
  }
  catch(std::exception& e) {
    Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().reset(new DebugLayout("Uncaught Exception of Type: "s + e.what()));
  }
  catch(...) {
    auto description = ExceptionTools::handle_eptr(std::current_exception());
    Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().reset(new DebugLayout(description));
  }


}

sigc::connection LayoutFolderMonitor::onChange(std::function<void()> cb)
{
  return m_onChange.connect(cb);
}

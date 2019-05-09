#include "LayoutFolderMonitor.h"
#include "LayoutFactory.h"
#include <Application.h>
#include <Options.h>
#include <proxies/hwui/debug-oled/DebugLayout.h>
#include <tools/ExceptionTools.h>
#include <tools/FileTools.h>
#include "LayoutParser.h"
#include "ControlRegistry.h"
#include "ControlParser.h"
#include "Styles.h"
#include "StyleParser.h"
#include "tools/SpawnCommandLine.h"
#include "ConsistencyChecker.h"

LayoutFolderMonitor& LayoutFolderMonitor::get()
{
  static LayoutFolderMonitor mon;
  return mon;
}

LayoutFolderMonitor::LayoutFolderMonitor()
    : m_rootFolder(Gio::File::create_for_path(Application::get().getOptions()->getLayoutFolder()))
    , m_recMonitor(m_rootFolder,
                   std::bind(&LayoutFolderMonitor::onFileChanged, this, std::placeholders::_1, std::placeholders::_2,
                             std::placeholders::_3))
{
  bruteForce();
}

void LayoutFolderMonitor::onFileChanged(const Glib::RefPtr<Gio::File>& o, const Glib::RefPtr<Gio::File>& n,
                                        Gio::FileMonitorEvent e)
{
  bruteForce();
}

void LayoutFolderMonitor::bruteForce()
{
  DescriptiveLayouts::BoledLayoutFactory::get().clear();
  DescriptiveLayouts::ControlRegistry::get().clear();
  DescriptiveLayouts::StyleSheet::get().clear();

  try
  {
    auto allFiles = m_recMonitor.getAllFilesInFolder(m_rootFolder);

    for(auto& file : allFiles)
    {
      auto path = FileTools::getFullPath(file);

      if(g_str_has_suffix(path.c_str(), ".json"))
      {
        DescriptiveLayouts::importControls(path);
        DescriptiveLayouts::importLayout(path);
        DescriptiveLayouts::importStyles(path);
      }
      else if(g_str_has_suffix(path.c_str(), ".yaml"))
      {
        auto tmpPath = "/tmp/__nl_style.json";
        SpawnCommandLine cmd("yaml2json " + path);
        g_file_set_contents(tmpPath, cmd.getStdOutput().c_str(), -1, nullptr);
        DescriptiveLayouts::importControls(tmpPath);
        DescriptiveLayouts::importLayout(tmpPath);
        DescriptiveLayouts::importStyles(tmpPath);
      }
    }

    DescriptiveLayouts::ConsistencyChecker checker(std::cout);
    checker.checkAll();
    m_onChange.send();
  }
  catch(ExceptionTools::TemplateException& e)
  {
    Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().reset(new DebugLayout(e.what() + e.where()));
  }
  catch(nlohmann::json::out_of_range& e)
  {
    Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().reset(new DebugLayout(e.what()));
  }
  catch(std::out_of_range& e)
  {
    Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().reset(new DebugLayout(e.what()));
  }
  catch(nlohmann::json::parse_error& e)
  {
    Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().reset(new DebugLayout(e.what()));
  }
  catch(std::runtime_error& e)
  {
    Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().reset(new DebugLayout(e.what()));
  }
  catch(std::exception& e)
  {
    Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().reset(
        new DebugLayout(std::string("Uncaught Exception of Type: ") + e.what()));
  }
  catch(...)
  {
    auto description = ExceptionTools::handle_eptr(std::current_exception());
    Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().reset(new DebugLayout(description));
  }
}

sigc::connection LayoutFolderMonitor::onChange(std::function<void()> cb)
{
  return m_onChange.connect(cb);
}

#include "Application.h"
#include "io/Bridges.h"
#include <fcntl.h>
#include "Options.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <nltools/messaging/Messaging.h>
#include <giomm.h>
#include <io/WiFiManager.h>
#include <io/USBManager.h>

#ifdef _DEVELOPMENT_PC
#include <ui/Window.h>
#endif

Application *Application::theApp = nullptr;

char *Application::initStatic(Application *app, char *argv)
{
  using namespace nltools::msg;

  theApp = app;
  Configuration conf;
  conf.offerEndpoints
      = { EndPoint::Lpc, EndPoint::Oled, EndPoint::PanelLed, EndPoint::RibbonLed, EndPoint::WiFiManager };
  conf.useEndpoints = { { EndPoint::Playground, app->getOptions()->getPlaygroundHost() },
                        { EndPoint::AudioEngine, app->getOptions()->getAudioEngineHost() } };
  nltools::msg::init(conf);
  return argv;
}

Application::Application(int numArgs, char **argv)
    : m_options(std::make_unique<Options>(numArgs, argv))
    , m_selfPath(initStatic(this, argv[0]))
    , m_bridges(std::make_unique<Bridges>())
    , m_wifiManager(std::make_unique<WiFiManager>())
    , m_usbManager(std::make_unique<USBManager>())
{
}

Application::~Application() = default;

Application &Application::get()
{
  return *theApp;
}

Glib::ustring Application::getSelfPath() const
{
  return m_selfPath;
}

#ifdef _DEVELOPMENT_PC

void Application::run()
{
  int numArgs = 0;
  char **argv = nullptr;
  auto app = Gtk::Application::create(numArgs, argv, "com.nonlinearlabs.bbbb");
  Window window;
  app->run(window);
}

#else

void Application::run()
{
  auto loop = Glib::MainLoop::create(true);
  loop->run();
}

#endif

Options *Application::getOptions()
{
  return m_options.get();
}

Bridges *Application::getBridges()
{
  return m_bridges.get();
}

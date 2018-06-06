#include "Application.h"
#include "io/Bridges.h"
#include "io/network/WebSocketServer.h"
#include <fcntl.h>
#include "Options.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ui/Window.h>

Application *Application::theApp = nullptr;

char *Application::initStatic(Application *app, char *argv)
{
  theApp = app;
  return argv;
}

Application::Application(int numArgs, char **argv) :
    m_selfPath(initStatic(this, argv[0])),
    m_options(std::make_unique<Options>(numArgs, argv)),
    m_websocketServer(std::make_unique<WebSocketServer>()),
    m_bridges(std::make_unique<Bridges>())
{
}

Application::~Application()
{
}

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

WebSocketServer *Application::getWebsocketServer()
{
  return m_websocketServer.get();
}

Bridges* Application::getBridges()
{
  return m_bridges.get();
}

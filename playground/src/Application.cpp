#include "Application.h"
#include "device-info/DeviceInformation.h"
#include "device-settings/DebugLevel.h"
#include "device-settings/Settings.h"
#include <device-settings/BlockingMainThreadIndication.h>
#include <fcntl.h>
#include "http/HTTPServer.h"
#include "http/UndoScope.h"
#include "hw-tests/HWTests.h"
#include "Options.h"
#include "presets/PresetManager.h"
#include "profiling/Profiler.h"
#include "proxies/hwui/debug-oled/DebugOLED.h"
#include "proxies/hwui/HWUI.h"
#include "proxies/lpc/LPCProxy.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <tools/WatchDog.h>
#include <unistd.h>
#include <clipboard/Clipboard.h>
#include <io/network/WebSocketSession.h>
#include <assert.h>
#include <proxies/hwui/debug-oled/DebugLayout.h>
#include <tools/ExceptionTools.h>

Application *Application::theApp = nullptr;

char *Application::initStatic(Application *app, char *argv)
{
  theApp = app;
  return argv;
}

void quitApp(int sig)
{
  DebugLevel::warning(__PRETTY_FUNCTION__, __LINE__, sig);
  Application::get().quit();
}

Application::Application(int numArgs, char **argv) :
    m_selfPath(initStatic(this, argv[0])),
    m_options(new Options(numArgs, argv)),
    m_theMainLoop(MainLoop::create()),
    m_websocketSession(std::make_unique<WebSocketSession>()),
    m_http(new HTTPServer()),
    m_settings(new Settings(m_http->getUpdateDocumentMaster())),
    m_undoScope(new UndoScope(m_http->getUpdateDocumentMaster())),
    m_presetManager(new PresetManager(m_http->getUpdateDocumentMaster())),
    m_lpcProxy(new LPCProxy()),
    m_hwui(new HWUI()),
    m_hwtests(new HWTests(m_http->getUpdateDocumentMaster())),
    m_watchDog(new WatchDog),
    m_aggroWatchDog(new WatchDog),
    m_deviceInformation(new DeviceInformation(m_http->getUpdateDocumentMaster())),
    m_clipboard(new Clipboard(m_http->getUpdateDocumentMaster())),
    m_heartbeatState(false),
    m_isQuit(false)
{
#ifdef _PROFILING
  Profiler::get().enable(true);
#endif

  m_settings->init();
  m_hwui->init();
  m_http->init();
  m_presetManager->init();
  m_hwui->setFocusAndMode(FocusAndMode(UIFocus::Parameters, UIMode::Select));
  runWatchDog();

  getMainContext()->signal_timeout().connect(sigc::mem_fun(this, &Application::heartbeat), 500);

  DebugLevel::warning(__PRETTY_FUNCTION__, __LINE__);

  ::signal(SIGQUIT, quitApp);
  ::signal(SIGTERM, quitApp);
  ::signal(SIGINT, quitApp);
}

Application::~Application()
{
  m_hwui->deInit();
  m_hwui.reset();
  m_presetManager.reset();

#ifdef _PROFILING
  Profiler::get().print();
#endif
}

Application &Application::get()
{
  return *theApp;
}

Glib::ustring Application::getSelfPath() const
{
  return m_selfPath;
}

Glib::ustring Application::getResourcePath() const
{
  RefPtr<Gio::File> app = Gio::File::create_for_path(m_selfPath);
  RefPtr<Gio::File> parent = app->get_parent();
  Glib::ustring parentPath = parent->get_path();
  return parentPath + "/resources/";
}

void Application::run()
{
  while(!m_isQuit) {
    DebugLevel::warning(__PRETTY_FUNCTION__);
    try {
      m_theMainLoop->run();
    } catch(...) {
      auto desc = ExceptionTools::handle_eptr(std::current_exception());
      getHWUI()->getPanelUnit().getEditPanel().getBoled().reset(new DebugLayout(desc));
    }
    DebugLevel::warning(__PRETTY_FUNCTION__);
  }
}

void Application::quit()
{
  DebugLevel::warning(__PRETTY_FUNCTION__);
  m_isQuit = true;
  m_theMainLoop->quit();
  DebugLevel::warning(__PRETTY_FUNCTION__);
}

bool Application::isQuit() const
{
  return m_isQuit;
}

RefPtr<MainContext> Application::getMainContext()
{
  return m_theMainLoop->get_context();
}

void Application::runWatchDog()
{
  if(m_watchDog)
    m_watchDog->run(std::chrono::seconds(2));

  if(m_aggroWatchDog)
  {
    m_aggroWatchDog->run(std::chrono::milliseconds(250), [ = ] (int numWarning, int inactiveFoMS)
    {
      DebugLevel::warning ("Aggro WatchDog was inactive for ", inactiveFoMS, "ms. Warning #", numWarning);

#ifdef _PROFILING
      Profiler::get().printAllCallstacks();
#endif

      if(auto h = getHWUI())
      {
        if(getSettings()->getSetting<BlockingMainThreadIndication>()->get())
        {
          h->indicateBlockingMainThread();
        }
      }
    });
  }
}

void Application::stopWatchDog()
{
  if(m_watchDog)
    m_watchDog->stop();

  if(m_aggroWatchDog)
    m_aggroWatchDog->stop();
}

shared_ptr<PresetManager> Application::getPresetManager() const
{
  return m_presetManager;
}

shared_ptr<HTTPServer> Application::getHTTPServer()
{
  return m_http;
}

shared_ptr<Clipboard> Application::getClipboard()
{
  return m_clipboard;
}

shared_ptr<Options> Application::getOptions()
{
  return m_options;
}

shared_ptr<LPCProxy> Application::getLPCProxy() const
{
  return m_lpcProxy;
}

shared_ptr<Settings> Application::getSettings()
{
  return m_settings;
}

shared_ptr<HWTests> Application::getHWTests()
{
  return m_hwtests;
}

shared_ptr<HWUI> Application::getHWUI()
{
  return m_hwui;
}

shared_ptr<const HWUI> Application::getHWUI() const
{
  return m_hwui;
}

shared_ptr<UndoScope> Application::getUndoScope()
{
  return m_undoScope;
}

shared_ptr<DeviceInformation> Application::getDeviceInformation()
{
  return m_deviceInformation;
}

WebSocketSession *Application::getWebSocketSession()
{
  return m_websocketSession.get();
}

bool Application::heartbeat()
{
  m_heartbeatState = !m_heartbeatState;

  const char *toSend = m_heartbeatState ? "1" : "0";

  int out = open("/sys/class/leds/playground_status/brightness", O_WRONLY);
  write(out, toSend, 1);
  fsync(out);
  close(out);

  if(m_heartbeatState)
  {
    int transactionsSinceLastCall = UNDO::Transaction::getAndResetNumTransactions();

    if(transactionsSinceLastCall)
      DebugLevel::info("Undo transactions are created at speed of:", transactionsSinceLastCall, "transactions/s");
  }

  return true;
}


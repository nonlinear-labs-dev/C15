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
#include "proxies/audio-engine/AudioEngineProxy.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <tools/WatchDog.h>
#include <unistd.h>
#include <clipboard/Clipboard.h>
#include <io/network/WebSocketSession.h>
#include <assert.h>
#include <proxies/hwui/debug-oled/DebugLayout.h>
#include <tools/ExceptionTools.h>
#include <nltools/messaging/Messaging.h>

Application *Application::theApp = nullptr;

void setupMessaging(const Options *options)
{
  using namespace nltools::msg;

  auto bbbb = options->getBBBB();
  auto ae = options->getAudioEngineHost();

  Configuration conf;
  conf.offerEndpoints = { EndPoint::Playground };
  conf.useEndpoints = {
    { EndPoint::Lpc, bbbb }, { EndPoint::Oled, bbbb }, { EndPoint::PanelLed, bbbb }, { EndPoint::RibbonLed, bbbb }
  };
  nltools::msg::init(conf);
}

std::unique_ptr<Options> Application::initStatic(Application *app, std::unique_ptr<Options> options)
{
  theApp = app;
  setupMessaging(options.get());
  return options;
}

void quitApp(int sig)
{
  DebugLevel::warning(__PRETTY_FUNCTION__, __LINE__, sig);
  Application::get().quit();
}

Application::Application(int numArgs, char **argv)
    : m_options(initStatic(this, std::make_unique<Options>(numArgs, argv)))
    , m_theMainLoop(MainLoop::create())
    , m_http(new HTTPServer())
    , m_settings(new Settings(m_http->getUpdateDocumentMaster()))
    , m_undoScope(new UndoScope(m_http->getUpdateDocumentMaster()))
    , m_presetManager(new PresetManager(m_http->getUpdateDocumentMaster()))
    , m_lpcProxy(new LPCProxy())
    , m_audioEngineProxy(new AudioEngineProxy)
    , m_hwui(new HWUI())
    , m_hwtests(new HWTests(m_http->getUpdateDocumentMaster()))
    , m_watchDog(new WatchDog)
    , m_aggroWatchDog(new WatchDog)
    , m_deviceInformation(new DeviceInformation(m_http->getUpdateDocumentMaster()))
    , m_clipboard(new Clipboard(m_http->getUpdateDocumentMaster()))
    , m_heartbeatState(false)
    , m_isQuit(false)
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
  DebugLevel::warning(__PRETTY_FUNCTION__, __LINE__);

  m_watchDog.reset();
  m_aggroWatchDog.reset();

  m_hwui->deInit();
  m_hwui.reset();
  m_undoScope.reset();
  m_presetManager.reset();
  DebugLevel::warning(__PRETTY_FUNCTION__, __LINE__);

#ifdef _PROFILING
  Profiler::get().print();
#endif
  DebugLevel::warning(__PRETTY_FUNCTION__, __LINE__);
}

Application &Application::get()
{
  return *theApp;
}

Glib::ustring Application::getSelfPath() const
{
  return getOptions()->getSelfPath();
}

Glib::ustring Application::getResourcePath() const
{
  RefPtr<Gio::File> app = Gio::File::create_for_path(getSelfPath());
  RefPtr<Gio::File> parent = app->get_parent();
  Glib::ustring parentPath = parent->get_path();
  return parentPath + "/resources/";
}

void Application::run()
{
  while(!m_isQuit)
  {
    DebugLevel::warning(__PRETTY_FUNCTION__);
    m_theMainLoop->run();
    DebugLevel::warning(__PRETTY_FUNCTION__);
  }
}

void Application::quit()
{
  DebugLevel::warning(__PRETTY_FUNCTION__);
  if(!m_isQuit)
  {
    m_isQuit = true;
    m_theMainLoop->quit();
  }
  else
  {
    DebugLevel::warning("Application already quit!");
    std::terminate();
  }
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
    m_aggroWatchDog->run(std::chrono::milliseconds(250), [=](int numWarning, int inactiveFoMS) {
      DebugLevel::warning("Aggro WatchDog was inactive for ", inactiveFoMS, "ms. Warning #", numWarning);

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

PresetManager *Application::getPresetManager() const
{
  return m_presetManager.get();
}

HTTPServer *Application::getHTTPServer()
{
  return m_http.get();
}

Clipboard *Application::getClipboard()
{
  return m_clipboard.get();
}

const Options *Application::getOptions() const
{
  return m_options.get();
}

LPCProxy *Application::getLPCProxy() const
{
  return m_lpcProxy.get();
}

AudioEngineProxy *Application::getAudioEngineProxy() const
{
  return m_audioEngineProxy.get();
}

Settings *Application::getSettings()
{
  return m_settings.get();
}

HWTests *Application::getHWTests()
{
  return m_hwtests.get();
}

HWUI *Application::getHWUI()
{
  return m_hwui.get();
}

const HWUI *Application::getHWUI() const
{
  return m_hwui.get();
}

UndoScope *Application::getUndoScope()
{
  return m_undoScope.get();
}

DeviceInformation *Application::getDeviceInformation()
{
  return m_deviceInformation.get();
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

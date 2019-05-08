#pragma once

#include "playground.h"

class DeviceInformation;
class WatchDog;
class HTTPServer;
class PresetManager;
class LPCProxy;
class HWUI;
class Options;
class HWTests;
class UndoScope;
class Settings;
class EmbeddedPC;
class Clipboard;
class WebSocketSession;

class Application
{
 public:
  Application(int numArgs, char **argv);
  virtual ~Application();

  Glib::ustring getSelfPath() const;
  Glib::ustring getResourcePath() const;

  static Application &get();

  void run();
  RefPtr<MainContext> getMainContext();

  shared_ptr<PresetManager> getPresetManager() const;
  shared_ptr<HTTPServer> getHTTPServer();
  shared_ptr<Options> getOptions();
  shared_ptr<LPCProxy> getLPCProxy() const;
  shared_ptr<HWTests> getHWTests();
  shared_ptr<HWUI> getHWUI();
  shared_ptr<const HWUI> getHWUI() const;
  shared_ptr<UndoScope> getUndoScope();
  shared_ptr<Settings> getSettings();
  shared_ptr<DeviceInformation> getDeviceInformation();
  shared_ptr<Clipboard> getClipboard();

  WebSocketSession *getWebSocketSession();

  void quit();
  bool isQuit() const;

  void runWatchDog();
  void stopWatchDog();

 private:
  bool heartbeat();
  static char *initStatic(Application *app, char *argv);

  static Application *theApp;
  Glib::ustring m_selfPath;
  shared_ptr<Options> m_options;
  RefPtr<MainLoop> m_theMainLoop;
  unique_ptr<WebSocketSession> m_websocketSession;
  shared_ptr<HTTPServer> m_http;
  shared_ptr<Settings> m_settings;
  shared_ptr<UndoScope> m_undoScope;
  shared_ptr<PresetManager> m_presetManager;
  shared_ptr<LPCProxy> m_lpcProxy;
  shared_ptr<HWUI> m_hwui;
  shared_ptr<HWTests> m_hwtests;
  shared_ptr<EmbeddedPC> m_epc;
  unique_ptr<WatchDog> m_watchDog;
  unique_ptr<WatchDog> m_aggroWatchDog;
  shared_ptr<DeviceInformation> m_deviceInformation;
  shared_ptr<Clipboard> m_clipboard;

  bool m_heartbeatState;
  bool m_isQuit;

  friend class AutoSaveInterval;
};

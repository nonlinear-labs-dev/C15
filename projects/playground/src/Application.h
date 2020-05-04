#pragma once

#include "playground.h"
#include <memory>
#include <glibmm/refptr.h>
#include <proxies/usb/USBChangeListener.h>

class DeviceInformation;
class WatchDog;
class HTTPServer;
class PresetManager;
class LPCProxy;
class AudioEngineProxy;
class HWUI;
class Options;
class UndoScope;
class Settings;
class EmbeddedPC;
class Clipboard;

namespace Glib
{
  class MainContext;
  class MainLoop;
}

class Application
{
 public:
  Application(int numArgs, char **argv);
  virtual ~Application();

  static Application &get();
  static bool exists();

  void run();
  Glib::RefPtr<Glib::MainContext> getMainContext();

  PresetManager *getPresetManager() const;
  HTTPServer *getHTTPServer();
  const Options *getOptions() const;
  LPCProxy *getLPCProxy() const;
  AudioEngineProxy *getAudioEngineProxy() const;
  HWUI *getHWUI();
  const HWUI *getHWUI() const;
  UndoScope *getUndoScope();
  Settings *getSettings();
  DeviceInformation *getDeviceInformation();
  Clipboard *getClipboard();

  void quit();
  bool isQuit() const;

  void runWatchDog();
  void stopWatchDog();

 private:
  bool heartbeat();
  static std::unique_ptr<Options> initStatic(Application *app, std::unique_ptr<Options> options);

  static Application *theApp;
  std::unique_ptr<Options> m_options;
  Glib::RefPtr<Glib::MainLoop> m_theMainLoop;

  std::unique_ptr<HTTPServer> m_http;
  std::unique_ptr<Settings> m_settings;
  std::unique_ptr<UndoScope> m_undoScope;
  std::unique_ptr<PresetManager> m_presetManager;
  std::unique_ptr<LPCProxy> m_lpcProxy;
  std::unique_ptr<AudioEngineProxy> m_audioEngineProxy;
  std::unique_ptr<HWUI> m_hwui;

  std::unique_ptr<WatchDog> m_watchDog;
  std::unique_ptr<WatchDog> m_aggroWatchDog;
  std::unique_ptr<DeviceInformation> m_deviceInformation;
  std::unique_ptr<Clipboard> m_clipboard;
  std::unique_ptr<USBChangeListener> m_usbChangeListener;

  bool m_heartbeatState;
  bool m_isQuit;
};

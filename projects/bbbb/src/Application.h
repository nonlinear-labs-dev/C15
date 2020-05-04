#pragma once

#include "bbbb.h"
#include <memory>
#include <glibmm/ustring.h>

class AudioEngineOptions;
class Bridges;
class WiFiManager;
class USBManager;

class Application
{
 public:
  Application(int numArgs, char **argv);
  virtual ~Application();

  Glib::ustring getSelfPath() const;

  static Application &get();

  void run();

  AudioEngineOptions *getOptions();
  Bridges *getBridges();

 private:
  static char *initStatic(Application *app, char *argv);

  static Application *theApp;

  std::unique_ptr<AudioEngineOptions> m_options;
  Glib::ustring m_selfPath;
  std::unique_ptr<Bridges> m_bridges;
  std::unique_ptr<WiFiManager> m_wifiManager;
  std::unique_ptr<USBManager> m_usbManager;
};

#pragma once

#include "bbbb.h"

class Options;
class Bridges;

class Application
{
 public:
  Application(int numArgs, char **argv);
  virtual ~Application();

  Glib::ustring getSelfPath() const;

  static Application &get();

  void run();

  Options *getOptions();
  Bridges *getBridges();

 private:
  static char *initStatic(Application *app, char *argv);

  static Application *theApp;

  std::unique_ptr<Options> m_options;
  Glib::ustring m_selfPath;
  std::unique_ptr<Bridges> m_bridges;
};

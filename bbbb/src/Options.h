#pragma once

#include "bbbb.h"
#include <giomm/file.h>

class Options
{
 public:
  Options(int &argc, char **&argv);

  Glib::ustring getPlaygroundHost() const;
  Glib::ustring getAudioEngineHost() const;

  bool doTimeStamps() const;

 private:
  bool m_doTimeStamps = false;

  Glib::ustring m_playgroundHost = "localhost";
  Glib::ustring m_audioengineHost = "localhost";
};

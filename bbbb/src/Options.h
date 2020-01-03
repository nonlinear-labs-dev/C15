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
  bool logLPCRaw() const;
  bool logHeartBeat() const;

 private:
  bool m_doTimeStamps = false;
  bool m_logLpcRaw = false;
  bool m_logHeartBeat = false;

  Glib::ustring m_playgroundHost = "localhost";
  Glib::ustring m_audioengineHost = "localhost";
};

#pragma once

#include <glibmm/ustring.h>

class Options
{
 public:
  Options(int &argc, char **&argv);

  Glib::ustring getAudioEngineHost() const;

 private:
  Glib::ustring m_audioengineHost = "192.168.10.10";
};

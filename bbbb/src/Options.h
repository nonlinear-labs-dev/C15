#pragma once

#include "bbbb.h"
#include <giomm/file.h>

class Options
{
 public:
  Options(int &argc, char **&argv);

  Glib::ustring getPlaygroundHost() const;

 private:
  Glib::ustring m_playgroundHost = "localhost";
};

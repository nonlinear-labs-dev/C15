#pragma once

#include "playground.h"

class FileSystem
{
 public:
  static void deleteFolder(Glib::RefPtr<Gio::File> pmFolder);
  static bool isNameAUUID(const ustring &name);
};

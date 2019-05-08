#pragma once

#include <xml/FileOutStream.h>

class CommitableFileOutStream : public FileOutStream
{
  typedef FileOutStream super;

 public:
  CommitableFileOutStream(const Glib::ustring &fileName, bool zip);
};

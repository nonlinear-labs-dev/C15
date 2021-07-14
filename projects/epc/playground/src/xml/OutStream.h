#pragma once

#include "playground.h"

class OutStream
{
 public:
  OutStream();
  virtual ~OutStream();

  void write(const Glib::ustring &str);
  void write(const void *buf, size_t numBytes);

 private:
  virtual void implWrite(const Glib::ustring &str) = 0;
  virtual void implWrite(const void *buf, size_t numBytes) = 0;
  [[nodiscard]] Glib::ustring toUTF8(const Glib::ustring &in) const;
};

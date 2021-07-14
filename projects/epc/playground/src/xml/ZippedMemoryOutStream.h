#pragma once

#include "OutStream.h"
#include <giomm.h>
#include <vector>

class ZippedMemoryOutStream : public OutStream
{
 public:
  ZippedMemoryOutStream();

  std::vector<uint8_t> &&exhaust();

 private:
  void implWrite(const Glib::ustring &str) override;
  void implWrite(const void *buf, size_t numBytes) override;

  Glib::RefPtr<Gio::ZlibCompressor> m_zip;
  std::vector<uint8_t> m_buffer;
};

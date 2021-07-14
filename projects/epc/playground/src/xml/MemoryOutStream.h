#pragma once

#include "OutStream.h"
#include <vector>

class MemoryOutStream : public OutStream
{
 public:
  MemoryOutStream();

  std::vector<uint8_t> &&exhaust();

 private:
  void implWrite(const Glib::ustring &str) override;
  void implWrite(const void *buf, size_t numBytes) override;

  std::vector<uint8_t> m_buffer;
};

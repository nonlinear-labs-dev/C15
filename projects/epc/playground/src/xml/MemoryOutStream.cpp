#include "MemoryOutStream.h"

MemoryOutStream::MemoryOutStream()
{
}

std::vector<uint8_t> &&MemoryOutStream::exhaust()
{
  return std::move(m_buffer);
}

void MemoryOutStream::implWrite(const Glib::ustring &str)
{
  implWrite(str.raw().data(), str.raw().size());
}

void MemoryOutStream::implWrite(const void *buf, size_t numBytes)
{
  auto src = reinterpret_cast<const uint8_t *>(buf);
  m_buffer.insert(m_buffer.end(), src, src + numBytes);
}

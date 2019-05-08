#pragma once

#include <xml/InStream.h>
#include <libsoup/soup.h>

class MemoryInStream : public InStream
{
 private:
  typedef InStream super;

 public:
  MemoryInStream(const Glib::ustring &mem, bool zipped);
  MemoryInStream(SoupBuffer *buffer, bool zipped);
  MemoryInStream(std::vector<uint8_t> buffer, bool zipped);
  virtual ~MemoryInStream();

  virtual bool eof() const;
  virtual ustring read() override;
  virtual vector<uint8_t> readAll() override;

  bool isBufferCorrectC15Backup();

 private:
  std::vector<uint8_t> m_buffer;
  GDataInputStream *m_stream = nullptr;
  bool m_eof = false;
};

#pragma once

#include "xml/OutStream.h"
#include <libsoup/soup.h>
#include <glibmm/refptr.h>
#include <vector>

class SoupOutStream : public OutStream
{
 public:
  SoupOutStream(SoupServer *server, SoupMessage *msg, const Glib::ustring &contentType, bool zip);
  ~SoupOutStream() override;

  void flush();

 private:
  void implWrite(const Glib::ustring &str) override;
  void implWrite(const void *buf, size_t numBytes) override;

  void push(const Glib::ustring &str);
  void push(const char *str, size_t numBytes);

  Glib::RefPtr<Gio::ZlibCompressor> m_zip;
  SoupServer *m_server = nullptr;
  SoupMessage *m_msg = nullptr;

  std::vector<char> m_scratchBuffer;
  bool m_completed = false;
};

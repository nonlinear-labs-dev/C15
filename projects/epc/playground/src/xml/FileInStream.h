#pragma once

#include "InStream.h"

struct _GDataInputStream;
using GDataInputStream = struct _GDataInputStream;

class FileInStream : public InStream
{
 public:
  FileInStream(const Glib::ustring &fileName, bool tryZip);
  ~FileInStream() override;

  Glib::ustring read() override;
  std::vector<uint8_t> readAll() override;
  [[nodiscard]] bool eof() const override;

 private:
  GDataInputStream *m_stream = nullptr;
  bool m_eof = false;
};

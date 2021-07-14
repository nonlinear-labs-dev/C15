#pragma once

#include "OutStream.h"
#include <memory>
#include <ostream>

class StandardOutStream : public OutStream
{
 public:
  explicit StandardOutStream(std::shared_ptr<std::ostream> out);

 private:
  void implWrite(const Glib::ustring &str) override;
  void implWrite(const void *buf, size_t numBytes) override;

  std::shared_ptr<std::ostream> m_out;
};

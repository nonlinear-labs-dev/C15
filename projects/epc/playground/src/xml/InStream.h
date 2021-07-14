#pragma once

#include "playground.h"
#include <vector>

class InStream
{
 public:
  InStream();
  virtual ~InStream();

  virtual Glib::ustring read() = 0;
  virtual std::vector<uint8_t> readAll() = 0;
  [[nodiscard]] virtual bool eof() const = 0;
};

#pragma once

#include "playground.h"

class InStream
{
 public:
  InStream();
  virtual ~InStream();

  virtual ustring read() = 0;
  virtual vector<uint8_t> readAll() = 0;
  virtual bool eof() const = 0;
};

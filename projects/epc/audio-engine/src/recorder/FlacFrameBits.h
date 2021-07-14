#pragma once

#include "Bitstream.h"

class FlacFrameBits
{
 public:
  FlacFrameBits(std::vector<uint8_t> &buffer);

  void patchFrameNumber(uint64_t n);

 private:
  Bitstream m_bits;
};

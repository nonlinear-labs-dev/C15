#pragma once

#include <vector>
#include <stdint.h>

struct Bitstream
{
  Bitstream(std::vector<uint8_t> &d);

  uint64_t read(uint64_t &pos, uint32_t numBits) const;
  uint8_t readLengthOfUtf8EncodedField(uint64_t pos) const;

  uint64_t readUtf8(uint64_t &pos) const;
  void patchUtf8(uint64_t &pos, uint64_t numBytes, uint64_t num);

  uint8_t crc8(uint64_t begin, uint64_t end) const;
  uint16_t crc16(uint64_t begin, uint64_t end) const;

  void patch(uint64_t &pos, uint32_t numBits, uint64_t num);

  uint64_t byteLength() const;

 private:
  uint64_t readBit(uint64_t &pos) const;
  void patchBit(uint64_t &pos, uint32_t i);

  std::vector<uint8_t> &m_data;
};
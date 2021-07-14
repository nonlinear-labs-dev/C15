#include "FlacFrameBits.h"

FlacFrameBits::FlacFrameBits(std::vector<uint8_t> &buffer)
    : m_bits(buffer)
{
}

void FlacFrameBits::patchFrameNumber(uint64_t n)
{
  uint64_t frameNumberBitPosition = 32;

  uint64_t numBytesUsedForFrameNumber = m_bits.readLengthOfUtf8EncodedField(frameNumberBitPosition);
  m_bits.patchUtf8(frameNumberBitPosition, numBytesUsedForFrameNumber, n);

  uint64_t headerCrcBytePos = 4 + numBytesUsedForFrameNumber;
  uint64_t headerCrcBitPos = headerCrcBytePos * 8;
  m_bits.patch(headerCrcBitPos, 8, m_bits.crc8(0, headerCrcBytePos));

  uint64_t frameCrcBytePos = m_bits.byteLength() - 2;
  uint64_t frameCrcBitPos = frameCrcBytePos * 8;
  m_bits.patch(frameCrcBitPos, 16, m_bits.crc16(0, frameCrcBytePos));
}

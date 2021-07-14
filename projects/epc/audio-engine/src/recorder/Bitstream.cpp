#include "Bitstream.h"
#include <array>

Bitstream::Bitstream(std::vector<uint8_t> &d)
    : m_data(d)
{
}

uint64_t Bitstream::read(uint64_t &pos, uint32_t numBits) const
{
  uint64_t ret = 0;
  for(uint32_t i = 0; i < numBits; i++)
    ret |= readBit(pos) << (numBits - i - 1);

  return ret;
}

void Bitstream::patch(uint64_t &pos, uint32_t numBits, uint64_t num)
{
  for(uint32_t i = 0; i < numBits; i++)
    patchBit(pos, (num >> (numBits - i - 1)) & 0x01);
}

uint64_t Bitstream::byteLength() const
{
  return m_data.size();
}

uint8_t Bitstream::readLengthOfUtf8EncodedField(uint64_t pos) const
{
  auto firstFrameNumberByte = read(pos, 8);

  if(!(firstFrameNumberByte & 0x80))
    return 1;

  // find first msb of 0
  auto numBytesUsed = 0;
  for(auto i = 7; i >= 0; i--)
  {
    if(!(firstFrameNumberByte & (1 << i)))
      break;

    numBytesUsed++;
  }

  return numBytesUsed;
}

uint64_t Bitstream::readUtf8(uint64_t &pos) const
{
  auto numBytesUsedForFrameNumber = readLengthOfUtf8EncodedField(pos);

  if(numBytesUsedForFrameNumber == 1)
    return read(pos, 8);

  uint8_t firstByteMask = 0;

  for(int i = 0; i < (8 - numBytesUsedForFrameNumber - 1); i++)
  {
    firstByteMask <<= 1;
    firstByteMask |= 0x01;
  }

  uint64_t ret = read(pos, 8) & firstByteMask;

  for(int i = 1; i < numBytesUsedForFrameNumber; i++)
  {
    ret <<= 6;
    ret |= read(pos, 8) & 0x3F;
  }
  return ret;
}

void Bitstream::patchUtf8(uint64_t &pos, uint64_t numBytes, uint64_t num)
{
  if(numBytes == 1)
  {
    patch(pos, 8, num & 0x7F);
  }
  else
  {
    uint8_t patchData[numBytes];

    patchData[numBytes - 1] = (num >> ((numBytes - 1) * 6));

    for(int i = 0; i < numBytes; i++)
    {
      patchData[numBytes - 1] |= (1 << (7 - i));
    }

    patchData[numBytes - 1] &= ~(1 << (7 - numBytes));

    for(int i = 0; i < numBytes - 1; i++)
    {
      patchData[i] = (num & 0x3F) | 0x80;
      num >>= 6;
    }

    for(int i = numBytes - 1; i >= 0; i--)
      patch(pos, 8, patchData[i]);
  }
}

uint8_t Bitstream::crc8(uint64_t begin, uint64_t end) const
{
  unsigned crc = 0;

  for(auto i = begin; i < end; i++)
  {
    crc ^= m_data[i] << 8;

    for(auto k = 0; k < 8; k++)
    {
      if(crc & 0x8000)
        crc ^= (0x1070 << 3);
      crc <<= 1;
    }
  }
  return (uint8_t)(crc >> 8);
}

std::array<uint16_t, 256> makeCRC16Table()
{
  constexpr auto mask = 0x8005;
  std::array<uint16_t, 256> ret;
  for(int i = 0; i < 256; i++)
  {
    uint16_t crc = i << 8;
    for(auto j = 0; j < 8; j++)
    {
      if(crc & 0x8000)
        crc = crc << 1 ^ mask;
      else
        crc <<= 1;
    }
    ret[i] = crc;
  }
  return ret;
}

uint16_t Bitstream::crc16(uint64_t begin, uint64_t end) const
{
  static auto table = makeCRC16Table();

  uint16_t crc = 0;
  for(int i = begin; i < end; i++)
    crc = crc << 8 ^ table[crc >> 8 ^ uint16_t(m_data[i])];

  return crc;
}

uint64_t Bitstream::readBit(uint64_t &pos) const
{
  auto byte = pos / 8;
  auto bit = 7 - (pos % 8);
  pos++;
  return (m_data[byte] >> bit) & 0x01;
}

void Bitstream::patchBit(uint64_t &pos, uint32_t i)
{
  auto byte = pos / 8;
  auto bit = 7 - (pos % 8);
  pos++;
  uint8_t mask = ~(1 << bit);
  m_data[byte] &= mask;
  m_data[byte] |= i << bit;
}

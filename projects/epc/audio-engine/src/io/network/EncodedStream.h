#pragma once

#include <io/RingBuffer.h>
#include <string>

class EncodedStream
{
 public:
  EncodedStream(const RingBuffer<SampleFrame> &ring, uint32_t sampleRate)
      : m_ring(ring)
      , m_sampleRate(sampleRate)
  {
  }

  virtual std::string getContentType() const = 0;
  virtual ~EncodedStream() = default;

 protected:
  const RingBuffer<SampleFrame> &m_ring;
  uint32_t m_sampleRate;
};

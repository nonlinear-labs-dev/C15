#pragma once

#include <io/RingBuffer.h>

class EncodedStream
{
 public:
  using AudioRing = RingBuffer<SampleFrame, 1 << 16>;

  EncodedStream(const AudioRing &ring, uint32_t sampleRate)
      : m_ring(ring)
      , m_sampleRate(sampleRate)
  {
  }

  virtual std::string getContentType() const = 0;
  virtual ~EncodedStream() = default;

 protected:
  const AudioRing &m_ring;
  uint32_t m_sampleRate;
};

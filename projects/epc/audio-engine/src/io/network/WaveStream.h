#pragma once

#include <Types.h>
#include "EncodedStream.h"
#include <sigc++/connection.h>
#include <functional>

class WaveStream : public EncodedStream
{
 public:
  using CB = std::function<void(const uint8_t *data, size_t numBytes)>;
  WaveStream(const RingBuffer<SampleFrame> &ring, uint32_t sampleRate, CB cb);
  ~WaveStream() override;

  std::string getContentType() const override;

 private:
  bool write();

  CB m_cb;
  uint64_t m_readHead = 0;
  sigc::connection m_connection;
};

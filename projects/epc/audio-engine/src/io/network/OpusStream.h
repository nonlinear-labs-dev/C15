#pragma once

#include <Types.h>
#include "EncodedStream.h"
#include <sigc++/connection.h>
#include <functional>
#include <opus/opus.h>

class OpusStream : public EncodedStream
{
 public:
  using CB = std::function<void(const uint8_t *data, size_t numBytes)>;
  OpusStream(const EncodedStream::AudioRing &ring, uint32_t sampleRate, CB cb);
  ~OpusStream() override;

  std::string getContentType() const override;

 private:
  bool writeToOpus();

  CB m_cb;
  OpusEncoder *m_encoder;
  sigc::connection m_connection;
  uint64_t m_readHead = 0;
};

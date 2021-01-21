#pragma once

#include <Types.h>
#include "EncodedStream.h"
#include <FLAC/stream_encoder.h>
#include <sigc++/connection.h>
#include <functional>

class FlacStream : public EncodedStream
{
 public:
  using CB = std::function<void(const uint8_t *data, size_t numBytes)>;
  FlacStream(const EncodedStream::AudioRing &ring, uint32_t sampleRate, CB cb);
  ~FlacStream() override;

  std::string getContentType() const override;

 private:
  bool writeToFlac();

  static FLAC__StreamEncoderWriteStatus writeToOut(const FLAC__StreamEncoder *encoder, const FLAC__byte buffer[],
                                                   size_t bytes, uint32_t samples, uint32_t current_frame,
                                                   void *client_data);

  CB m_cb;
  FLAC__StreamEncoder *m_encoder = nullptr;
  uint64_t m_readHead = 0;
  sigc::connection m_connection;
};

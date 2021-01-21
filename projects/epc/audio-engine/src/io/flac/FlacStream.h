#pragma once

#include <Types.h>
#include <libsoup/soup-message.h>
#include <FLAC/stream_encoder.h>
#include <functional>
#include <sigc++/connection.h>

class Synth;

class FlacStream
{
 public:
  using CB = std::function<void(const uint8_t *data, size_t numBytes)>;
  FlacStream(const Synth *synth, CB cb);
  ~FlacStream();

 private:
  bool writeToFlac();

  static FLAC__StreamEncoderWriteStatus writeToOut(const FLAC__StreamEncoder *encoder, const FLAC__byte buffer[],
                                                   size_t bytes, uint32_t samples, uint32_t current_frame,
                                                   void *client_data);

  const Synth *m_synth;
  CB m_cb;
  std::function<void()> m_unPause;
  FLAC__StreamEncoder *m_encoder = nullptr;
  uint64_t m_readHead = 0;
  sigc::connection m_connection;
  std::vector<SampleFrame> m_scratch;
  std::vector<FLAC__int32> m_scratchFlac;
};

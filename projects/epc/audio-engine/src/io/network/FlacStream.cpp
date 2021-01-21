#include "FlacStream.h"
#include <glibmm.h>

FlacStream::FlacStream(const EncodedStream::AudioRing &ring, uint32_t sampleRate, CB cb)
    : EncodedStream(ring, sampleRate)
    , m_cb(cb)
    , m_encoder(FLAC__stream_encoder_new())
{
  m_readHead = m_ring.getWriteHead() - m_sampleRate / 4;

  FLAC__stream_encoder_set_channels(m_encoder, 2);
  FLAC__stream_encoder_set_sample_rate(m_encoder, m_sampleRate);
  FLAC__stream_encoder_set_bits_per_sample(m_encoder, 16);
  FLAC__stream_encoder_set_streamable_subset(m_encoder, true);
  FLAC__stream_encoder_init_stream(m_encoder, &writeToOut, nullptr, nullptr, nullptr, this);

  auto ctx = Glib::MainContext::get_default();
  m_connection = ctx->signal_timeout().connect(sigc::mem_fun(this, &FlacStream::writeToFlac), 100);
}

FlacStream::~FlacStream()
{
  m_connection.disconnect();
  FLAC__stream_encoder_delete(m_encoder);
}

std::string FlacStream::getContentType() const
{
  return "audio/x-flac";
}

FLAC__StreamEncoderWriteStatus FlacStream::writeToOut(const FLAC__StreamEncoder *, const FLAC__byte buffer[],
                                                      size_t bytes, uint32_t numSamples, uint32_t, void *client_data)
{
  static_cast<FlacStream *>(client_data)->m_cb(buffer, bytes);
  return FLAC__STREAM_ENCODER_WRITE_STATUS_OK;
}

bool FlacStream::writeToFlac()
{
  constexpr auto mul = std::numeric_limits<int16_t>::max() - 1;
  SampleFrame scratch[m_sampleRate];

  auto numFramesRead = m_ring.pop(scratch, m_sampleRate, m_readHead);
  m_readHead += numFramesRead;

  FLAC__int32 scratchFlac[m_sampleRate];
  auto out = scratchFlac;

  for(size_t i = 0; i < numFramesRead; i++)
  {
    *(out++) = static_cast<FLAC__int32>(scratch[i].left * mul);
    *(out++) = static_cast<FLAC__int32>(scratch[i].right * mul);
  }

  FLAC__stream_encoder_process_interleaved(m_encoder, scratchFlac, numFramesRead);
  return true;
}

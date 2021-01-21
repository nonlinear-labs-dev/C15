#include "FlacStream.h"
#include <synth/Synth.h>
#include <io/RingBuffer.h>
#include <Types.h>
#include <AudioEngineOptions.h>
#include <libsoup/soup-message.h>
#include <nltools/logging/Log.h>
#include <glibmm.h>

using namespace std::chrono_literals;

FlacStream::FlacStream(const Synth *synth, CB cb)
    : m_synth(synth)
    , m_cb(cb)
    , m_encoder(FLAC__stream_encoder_new())
    , m_readHead(m_synth->getAudioRing().getWriteHead())
{
  auto sr = static_cast<uint32_t>(m_synth->getOptions()->getSampleRate());
  auto delay = 125ms;

  FLAC__stream_encoder_set_channels(m_encoder, 2);
  FLAC__stream_encoder_set_sample_rate(m_encoder, sr);
  FLAC__stream_encoder_set_bits_per_sample(m_encoder, 16);
  FLAC__stream_encoder_set_streamable_subset(m_encoder, true);
  FLAC__stream_encoder_init_stream(m_encoder, &writeToOut, nullptr, nullptr, nullptr, this);

  auto ctx = Glib::MainContext::get_default();
  m_connection = ctx->signal_timeout().connect(sigc::mem_fun(this, &FlacStream::writeToFlac),
                                               static_cast<unsigned int>(delay.count()));
}

FlacStream::~FlacStream()
{
  m_connection.disconnect();
  FLAC__stream_encoder_delete(m_encoder);
}

FLAC__StreamEncoderWriteStatus FlacStream::writeToOut(const FLAC__StreamEncoder *, const FLAC__byte buffer[],
                                                      size_t bytes, uint32_t, uint32_t, void *client_data)
{
  static_cast<FlacStream *>(client_data)->m_cb(buffer, bytes);
  nltools::Log::warning("Wrote to soup:", bytes);
  return FLAC__STREAM_ENCODER_WRITE_STATUS_OK;
}

bool FlacStream::writeToFlac()
{
  constexpr auto mul = std::numeric_limits<int16_t>::max() - 1;

  auto numFrames = m_synth->getOptions()->getSampleRate();

  auto &ring = m_synth->getAudioRing();
  m_scratch.resize(numFrames);

  nltools::Log::warning("readHead:", m_readHead);
  nltools::Log::warning("writeHead:", ring.getWriteHead());

  auto numFramesRead = ring.pop(m_scratch.data(), numFrames, m_readHead);
  m_readHead += numFramesRead;

  nltools::Log::warning("numFramesRead:", numFramesRead);

  m_scratchFlac.resize(numFramesRead * 2);
  auto out = m_scratchFlac.data();

  for(size_t i = 0; i < numFramesRead; i++)
  {
    *(out++) = static_cast<FLAC__int32>(m_scratch[i].left * mul);
    *(out++) = static_cast<FLAC__int32>(m_scratch[i].right * mul);
  }

  FLAC__stream_encoder_process_interleaved(m_encoder, m_scratchFlac.data(), numFramesRead);
  return true;
}

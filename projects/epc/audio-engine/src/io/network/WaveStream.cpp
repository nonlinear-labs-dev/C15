#include "WaveStream.h"
#include <glibmm.h>
#include <nltools/logging/Log.h>

WaveStream::WaveStream(const RingBuffer<SampleFrame> &ring, uint32_t sampleRate, CB cb)
    : EncodedStream(ring, sampleRate)
    , m_cb(cb)
{
  m_readHead = m_ring.getWriteHead() - m_sampleRate / 4;
  m_connection
      = Glib::MainContext::get_default()->signal_timeout().connect(sigc::mem_fun(this, &WaveStream::write), 100);

  struct WaveHeader
  {
    char riffChunk[4] = { 'R', 'I', 'F', 'F' };
    uint32_t fileSize = std::numeric_limits<uint32_t>::max();
    char waveChunk[4] = { 'W', 'A', 'V', 'E' };
    char fmtChunk[4] = { 'f', 'm', 't', ' ' };
    uint32_t fmtSize = 16;
    uint16_t formatTag = 1;
    uint16_t channels = 2;
    uint32_t sampleRate = 0;
    uint32_t frameRate = 0;
    uint16_t frameSize = 4;
    uint16_t bitsPerSample = 16;
    char dataChunk[4] = { 'd', 'a', 't', 'a' };
    uint32_t dataSize = std::numeric_limits<uint32_t>::max();
  };

  WaveHeader h;
  h.sampleRate = sampleRate;
  h.frameRate = sampleRate * 2 * 2;

  m_cb(reinterpret_cast<uint8_t *>(&h), sizeof(WaveHeader));
}

WaveStream::~WaveStream()
{
  m_connection.disconnect();
}

std::string WaveStream::getContentType() const
{
  return "audio/x-wave";
}

bool WaveStream::write()
{
  constexpr auto mul = std::numeric_limits<int16_t>::max() - 1;
  SampleFrame scratch[m_sampleRate];

  auto numFramesRead = m_ring.pop(scratch, m_sampleRate, m_readHead);
  m_readHead += numFramesRead;

  nltools::Log::warning("NumFrames: ", numFramesRead);

  int16_t samples[numFramesRead * 2];
  auto walker = samples;

  for(size_t i = 0; i < numFramesRead; i++)
  {
    *(walker++) = static_cast<int16_t>(scratch[i].left * mul);
    *(walker++) = static_cast<int16_t>(scratch[i].right * mul);
  }

  m_cb(reinterpret_cast<uint8_t *>(samples), numFramesRead * 2 * 2);
  return true;
}

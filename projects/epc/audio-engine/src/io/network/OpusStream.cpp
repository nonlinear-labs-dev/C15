#include "OpusStream.h"
#include <glibmm.h>

OpusStream::OpusStream(const RingBuffer<SampleFrame> &ring, uint32_t sampleRate, CB cb)
    : EncodedStream(ring, sampleRate)
    , m_cb(cb)
{
  int error;
  m_encoder = opus_encoder_create(sampleRate, 2, OPUS_APPLICATION_AUDIO, &error);

  auto ctx = Glib::MainContext::get_default();
  m_connection = ctx->signal_timeout().connect(sigc::mem_fun(this, &OpusStream::writeToOpus), 100);
}

OpusStream::~OpusStream()
{
  m_connection.disconnect();
  opus_encoder_destroy(m_encoder);
}

std::string OpusStream::getContentType() const
{
  return "audio/x-opus";
}

bool OpusStream::writeToOpus()
{
  auto frameSize = 480;
  std::vector<SampleFrame> scratch(frameSize);

  while(m_ring.getWriteHead() - m_readHead >= frameSize)
  {
    auto numFramesRead = m_ring.pop(scratch.data(), frameSize, m_readHead);
    m_readHead += numFramesRead;

    std::vector<uint8_t> out(8192);

    auto asFloat = reinterpret_cast<float *>(scratch.data());

    if(auto res = opus_encode_float(m_encoder, asFloat, numFramesRead, out.data(), out.size()))
    {
      if(res > 0)
        m_cb(out.data(), res);
    }
  }

  return true;
}

#include "FlacEncoder.h"
#include <algorithm>

FlacEncoder::Frame::Frame(const FLAC__byte *buffer, size_t bytes, Sample max)
    : buffer { buffer, buffer + bytes }
    , recordTime { std::chrono::system_clock::now() }
    , id(getNextId())
    , max(std::min(max, 1.0f) * std::numeric_limits<decltype(this->max)>::max())
{
}

uint64_t FlacEncoder::Frame::getMemUsage() const
{
  return sizeof(Frame) + buffer.size();
}

uint64_t FlacEncoder::Frame::getNextId()
{
  static uint64_t max = 0;
  return max++;
}

nlohmann::json FlacEncoder::Frame::generateInfo() const
{
  return { { "id", id }, { "max", max }, { "record-time", recordTime.time_since_epoch().count() } };
}

FlacEncoder::FlacEncoder(int sr, std::function<void(std::unique_ptr<Frame>)> cb)
    : m_encoder(FLAC__stream_encoder_new())
    , m_cb(cb)
{
  FLAC__stream_encoder_set_channels(m_encoder, 2);
  FLAC__stream_encoder_set_sample_rate(m_encoder, sr);
  FLAC__stream_encoder_set_bits_per_sample(m_encoder, 24);
  FLAC__stream_encoder_set_streamable_subset(m_encoder, true);
  FLAC__stream_encoder_set_blocksize(m_encoder, flacFrameSize);
  FLAC__stream_encoder_init_stream(m_encoder, &writeToOut, nullptr, nullptr, nullptr, this);
}

FlacEncoder::~FlacEncoder()
{
  FLAC__stream_encoder_delete(m_encoder);
}

FLAC__StreamEncoderWriteStatus FlacEncoder::writeToOut(const FLAC__StreamEncoder *, const FLAC__byte buffer[],
                                                       size_t bytes, uint32_t numSamples, uint32_t, void *client_data)
{
  auto pThis = static_cast<FlacEncoder *>(client_data);
  auto frame = std::make_unique<Frame>(buffer, bytes, std::exchange(pThis->m_currentMax, 0));
  pThis->m_cb(std::move(frame));
  return FLAC__STREAM_ENCODER_WRITE_STATUS_OK;
}

void FlacEncoder::push(const SampleFrame *frames, size_t numFrames)
{
  constexpr auto mul = (1 << 23) - 1;
  const auto numSamples = 2 * numFrames;
  FLAC__int32 scratchFlac[numSamples];
  auto in = reinterpret_cast<const Sample *>(frames);
  auto minmax = std::minmax_element(in, in + numSamples);
  m_currentMax = std::max({ std::abs(*minmax.first), std::abs(*minmax.second), m_currentMax });
  std::transform(in, in + numSamples, scratchFlac, [](auto i) { return static_cast<FLAC__int32>(i * mul); });
  FLAC__stream_encoder_process_interleaved(m_encoder, scratchFlac, numFrames);
}

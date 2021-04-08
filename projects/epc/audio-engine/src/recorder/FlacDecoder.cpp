#include "FlacDecoder.h"
#include <nltools/logging/Log.h>
#include <glib.h>

FlacDecoder::FlacDecoder(FlacFrameStorage *s, FrameId begin, FrameId end)
    : m_storage(s)
    , m_info { begin, 0, end }
    , m_decoder(FLAC__stream_decoder_new())
    , m_streamOfFrames(m_storage->startStream(begin, end))
{
  FLAC__stream_decoder_init_stream(m_decoder, &readCB, nullptr, nullptr, nullptr, nullptr, &writeCB, nullptr, &errorCB,
                                   this);
}

FlacDecoder::~FlacDecoder()
{
  FLAC__stream_decoder_delete(m_decoder);
}

FlacDecoder::PositionInfo FlacDecoder::getPositionInfo() const
{
  auto ret = m_info;
  std::get<1>(ret) = m_numDecodedSamples / FlacEncoder::flacFrameSize + m_firstFrameDecoded;
  return ret;
}

FLAC__StreamDecoderReadStatus FlacDecoder::readCB(const FLAC__StreamDecoder *decoder, FLAC__byte buffer[],
                                                  size_t *bytes, void *client_data)
{
  auto pThis = static_cast<FlacDecoder *>(client_data);

  if(pThis->m_headerPos < 3)
  {
    auto &frame = pThis->m_storage->getHeaders()[pThis->m_headerPos++];
    std::copy(frame->buffer.begin(), frame->buffer.end(), buffer);
    *bytes = frame->buffer.size();
    return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
  }

  if(pThis->m_readScratch.empty())
  {
    pThis->m_streamOfFrames->next([&](const auto &frame, auto) {
      pThis->m_readScratch.insert(pThis->m_readScratch.end(), frame.buffer.begin(), frame.buffer.end());

      if(!pThis->m_firstFrameDecoded)
        pThis->m_firstFrameDecoded = frame.id;
    });
  }

  if(!pThis->m_readScratch.empty())
  {
    auto todo = std::min(pThis->m_readScratch.size(), *bytes);
    std::copy(pThis->m_readScratch.begin(), pThis->m_readScratch.begin() + todo, buffer);
    pThis->m_readScratch.erase(pThis->m_readScratch.begin(), pThis->m_readScratch.begin() + todo);
    *bytes = todo;
    return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
  }

  return FLAC__STREAM_DECODER_READ_STATUS_ABORT;
}

FLAC__StreamDecoderWriteStatus FlacDecoder::writeCB(const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame,
                                                    const FLAC__int32 *const buffer[], void *client_data)
{
  auto pThis = static_cast<FlacDecoder *>(client_data);
  pThis->m_audioDecoded = true;
  constexpr auto mul = 1.0f / ((1 << 23) - 1);

  auto left = buffer[0];
  auto right = buffer[1];

  for(size_t i = 0; i < FlacEncoder::flacFrameSize; i++)
  {
    pThis->m_currentTarget[i].left = std::clamp(left[i] * mul, -1.0f, 1.0f);
    pThis->m_currentTarget[i].right = std::clamp(right[i] * mul, -1.0f, 1.0f);
  }

  return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void FlacDecoder::errorCB(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data)
{
  nltools::Log::error(__LINE__, status);
}

size_t FlacDecoder::popAudio(SampleFrame *target, size_t maxNumFrames)
{
  auto todo = maxNumFrames;
  auto done = 0;

  while(todo >= FlacEncoder::flacFrameSize)
  {
    if(m_readScratch.empty() && m_streamOfFrames->eos())
      break;

    m_currentTarget = target;

    auto ret = FLAC__stream_decoder_process_single(m_decoder);

    if(m_audioDecoded && ret)
    {
      target += FlacEncoder::flacFrameSize;
      todo -= FlacEncoder::flacFrameSize;
      done += FlacEncoder::flacFrameSize;
      m_numDecodedSamples += FlacEncoder::flacFrameSize;
    }
  }

  return done;
}

bool FlacDecoder::eos() const
{
  return (m_readScratch.empty() && m_streamOfFrames->eos());
}

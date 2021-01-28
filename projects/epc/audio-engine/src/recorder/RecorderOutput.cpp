#include "RecorderOutput.h"
#include "FlacDecoder.h"

RecorderOutput::RecorderOutput(FlacFrameStorage *storage, int sr)
    : m_storage(storage)
    , m_sampleRate(sr)
    , m_ring(sr)
    , m_bgTask(std::async(std::launch::async, [this] { background(); }))
{
}

RecorderOutput::~RecorderOutput()
{
  m_close = true;
  m_cond.notify_one();
  m_bgTask.wait();
}

void RecorderOutput::process(SampleFrame *frames, size_t numFrames)
{
  SampleFrame buf[numFrames];
  auto n = m_ring.pop(buf, numFrames);

  for(size_t i = 0; i < n; i++)
  {
    frames[i].left += buf[i].left;
    frames[i].right += buf[i].right;
  }
}

void RecorderOutput::pause()
{
  std::unique_lock<std::mutex> l(m_mutex);
  m_paused = true;
  m_cond.notify_one();
}

void RecorderOutput::start(FrameId begin, FrameId end)
{
  std::unique_lock<std::mutex> l(m_mutex);
  m_decoder = std::make_unique<FlacDecoder>(m_storage, begin, end);
  m_paused = false;
  m_ring.reset();
  m_cond.notify_one();
}

nlohmann::json RecorderOutput::generateInfo()
{
  FlacDecoder::PositionInfo info = m_decoder ? m_decoder->getPositionInfo() : FlacDecoder::PositionInfo {};

  return {
    { "paused", m_paused }, { "begin", std::get<0>(info) }, { "pos", std::get<1>(info) }, { "end", std::get<2>(info) }
  };
}

void RecorderOutput::background()
{
  using namespace std::chrono_literals;
  std::unique_lock<std::mutex> l(m_mutex);

  SampleFrame buf[m_sampleRate];

  while(!m_close)
  {
    m_cond.wait_for(l, 100ms);

    if(!m_close && !m_paused && m_decoder)
    {
      auto todo = std::min(m_ring.getFreeSpace(), m_sampleRate);

      if(auto l = m_decoder->popAudio(buf, todo))
        m_ring.push(buf, l);
    }
  }
}

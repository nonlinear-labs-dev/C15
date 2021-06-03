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

  auto todo = numFrames;

  while(todo)
  {
    auto n = m_ring.pop(buf, todo);

    if(n == 0)
      break;

    for(size_t i = 0; i < n; i++)
    {
      frames[i].left += buf[i].left;
      frames[i].right += buf[i].right;
    }

    frames += n;
    todo -= n;
  }

  m_cond.notify_one();
}

void RecorderOutput::pause()
{
  std::unique_lock<std::mutex> l(m_mutex);
  m_paused = true;

  if(m_decoder)
  {
    auto framesInRing = m_ring.size() / FlacEncoder::flacFrameSize;
    m_requestedPlayPosition = std::get<1>(m_decoder->getPositionInfo()) - framesInRing;
  }

  m_cond.notify_one();
}

void RecorderOutput::setPlayPos(FrameId id)
{
  std::unique_lock<std::mutex> l(m_mutex);

  if(m_storage->getFrames().empty())
  {
    m_requestedPlayPosition = -1;
    return;
  }

  m_requestedPlayPosition = std::clamp(id, m_storage->getFrames().front()->id, m_storage->getFrames().back()->id);

  if(!m_paused)
  {
    l.unlock();
    start();
  }
}

void RecorderOutput::start()
{
  std::unique_lock<std::mutex> l(m_mutex);
  m_decoder = std::make_unique<FlacDecoder>(m_storage, m_requestedPlayPosition, std::numeric_limits<FrameId>::max());
  m_paused = false;
  m_ring.reset();
  m_cond.notify_one();
}

nlohmann::json RecorderOutput::generateInfo()
{
  FlacDecoder::PositionInfo info = m_decoder ? m_decoder->getPositionInfo() : FlacDecoder::PositionInfo {};
  auto framesInRing = m_ring.size() / FlacEncoder::flacFrameSize;
  int64_t playPos = m_paused ? m_requestedPlayPosition : (std::get<1>(info) - framesInRing);

  return { { "paused", m_paused }, { "begin", std::get<0>(info) }, { "pos", playPos }, { "end", std::get<2>(info) } };
}

void RecorderOutput::TEST_waitForBuffersFilled(size_t numFramesNeeded) const
{
  while(m_ring.avail() < numFramesNeeded)
  {
    std::this_thread::yield();
  }
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
      else if(m_decoder->eos())
        m_paused = true;
    }
  }
}

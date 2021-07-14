#include "RecorderInput.h"
#include "FlacEncoder.h"
#include "FlacFrameStorage.h"

RecorderInput::RecorderInput(FlacFrameStorage *storage, int sr)
    : m_storage(storage)
    , m_encoder(std::make_unique<FlacEncoder>(
          sr, [this, storage](auto frame, auto isHeader) { storage->push(std::move(frame), isHeader); }))
    , m_ring(sr)
    , m_bgTask(std::async(std::launch::async, [this] { background(); }))
{
}

RecorderInput::~RecorderInput()
{
  m_close = true;
  m_cond.notify_one();
  m_bgTask.wait();
}

void RecorderInput::process(SampleFrame *frames, size_t numFrames)
{
  m_ring.push(frames, numFrames);
  m_cond.notify_one();
}

void RecorderInput::togglePause()
{
  setPaused(!m_paused);
}

void RecorderInput::setPaused(bool p)
{
  std::unique_lock<std::mutex> l(m_mutex);
  m_paused = p;

  if(!m_paused)
  {
    m_ring.reset();
    m_encoder->resume();
  }
}

nlohmann::json RecorderInput::generateInfo()
{
  return { { "paused", m_paused } };
}

void RecorderInput::TEST_waitForSettling() const
{
  while(m_ring.avail() >= FlacEncoder::flacFrameSize)
    std::this_thread::yield();
}

void RecorderInput::background()
{
  std::unique_lock<std::mutex> l(m_mutex);
  SampleFrame samples[FlacEncoder::flacFrameSize];

  while(!m_close)
  {
    m_cond.wait(l);

    while(!m_close && m_ring.avail() >= FlacEncoder::flacFrameSize)
    {
      auto s = m_ring.pop(samples, FlacEncoder::flacFrameSize);

      if(!m_paused)
        m_encoder->push(samples, s);
    }
  }
}

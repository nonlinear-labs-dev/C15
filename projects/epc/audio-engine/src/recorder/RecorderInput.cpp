#include "RecorderInput.h"
#include "FlacEncoder.h"
#include "FlacFrameStorage.h"

RecorderInput::RecorderInput(FlacFrameStorage *storage, int sr)
    : storage(storage)
    , encoder(std::make_unique<FlacEncoder>(sr, [this, storage](auto frame) { storage->push(std::move(frame)); }))
    , ring(sr)
    , bgTask(std::async(std::launch::async, [this] { background(); }))
{
}

RecorderInput::~RecorderInput()
{
  close = true;
  cond.notify_one();
  bgTask.wait();
}

void RecorderInput::process(SampleFrame *frames, size_t numFrames)
{
  ring.push(frames, numFrames);
  cond.notify_one();
}

void RecorderInput::togglePause()
{
  paused = !paused;
}

nlohmann::json RecorderInput::generateInfo()
{
  return { { "paused", paused } };
}

void RecorderInput::TEST_waitForSettling() const
{
  while(ring.avail() >= FlacEncoder::flacFrameSize)
    std::this_thread::yield();
}

void RecorderInput::background()
{
  std::unique_lock<std::mutex> l(mutex);
  SampleFrame samples[FlacEncoder::flacFrameSize];

  while(!close)
  {
    cond.wait(l);

    while(!close && ring.avail() >= FlacEncoder::flacFrameSize)
    {
      auto s = ring.pop(samples, FlacEncoder::flacFrameSize);

      if(!paused)
        encoder->push(samples, s);
    }
  }
}

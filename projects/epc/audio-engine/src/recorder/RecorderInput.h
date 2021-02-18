#pragma once

#include <io/RingBuffer.h>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <future>
#include <nltools/nlohmann/json.hpp>

class FlacFrameStorage;
class FlacEncoder;

class RecorderInput
{
 public:
  RecorderInput(FlacFrameStorage *storage, int sr);
  ~RecorderInput();

  void process(SampleFrame *frames, size_t numFrames);
  void togglePause();

  nlohmann::json generateInfo();

  void TEST_waitForSettling() const;

 private:
  void background();

  FlacFrameStorage *storage;
  std::unique_ptr<FlacEncoder> encoder;
  RingBuffer<SampleFrame> ring;
  std::condition_variable cond;
  std::mutex mutex;
  bool close = false;
  std::future<void> bgTask;
  bool paused = false;
};

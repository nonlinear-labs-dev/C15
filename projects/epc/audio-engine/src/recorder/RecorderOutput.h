#pragma once

#include <io/RingBuffer.h>
#include <chrono>
#include <memory>
#include <condition_variable>
#include <mutex>
#include <future>
#include <nltools/nlohmann/json.hpp>

class FlacFrameStorage;
class FlacDecoder;

class RecorderOutput
{
 public:
  RecorderOutput(FlacFrameStorage *m_storage, int sr);
  ~RecorderOutput();

  void process(SampleFrame *frames, size_t numFrames);
  void pause();
  void start(FrameId begin, FrameId end);
  nlohmann::json generateInfo();

  void TEST_waitForBuffersFilled(size_t numFramesNeeded) const;

 private:
  void background();

  FlacFrameStorage *m_storage;
  size_t m_sampleRate;

  RingBuffer<SampleFrame> m_ring;
  std::unique_ptr<FlacDecoder> m_decoder;

  std::condition_variable m_cond;
  mutable std::mutex m_mutex;
  bool m_close = false;
  std::future<void> m_bgTask;
  bool m_paused = true;
};

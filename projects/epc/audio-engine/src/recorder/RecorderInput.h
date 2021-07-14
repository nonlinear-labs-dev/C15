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

  void setPaused(bool p);
  void process(SampleFrame *frames, size_t numFrames);
  void togglePause();

  nlohmann::json generateInfo();

  void TEST_waitForSettling() const;

 private:
  void background();

  FlacFrameStorage *m_storage;
  std::unique_ptr<FlacEncoder> m_encoder;
  RingBuffer<SampleFrame> m_ring;
  std::condition_variable m_cond;
  std::mutex m_mutex;
  bool m_close = false;
  std::future<void> m_bgTask;
  bool m_paused = false;
};

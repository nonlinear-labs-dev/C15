#pragma once

#include "Types.h"
#include "AudioOutput.h"
#include <asoundlib.h>
#include <functional>
#include <thread>
#include <memory>

class AudioWriterBase;
class HighPriorityTask;

class AlsaAudioOutput : public AudioOutput
{
 public:
  using Callback = std::function<void(SampleFrame *target, size_t numFrames)>;

  AlsaAudioOutput(const std::string &deviceName, Callback cb);
  ~AlsaAudioOutput();

  void start() override;
  void stop() override;

  double getPerformance() const override;

 private:
  void open(const std::string &deviceName);

  void close();
  void doBackgroundWork();
  void handleWriteError(snd_pcm_sframes_t result);
  void playback(SampleFrame *frames, size_t numFrames);
  template <typename T> snd_pcm_sframes_t playbackIntLE(const SampleFrame *frames, size_t numFrames);
  snd_pcm_sframes_t playbackF32(SampleFrame *frames, size_t numFrames);
  snd_pcm_sframes_t playbackInt24LE(const SampleFrame *frames, size_t numFrames);

  Callback m_cb;
  snd_pcm_t *m_handle = nullptr;
  std::unique_ptr<HighPriorityTask> m_task;
  bool m_run = true;

  snd_pcm_uframes_t m_framesProcessed = 0;
  snd_pcm_uframes_t m_numFramesPerPeriod = 0;

  std::unique_ptr<AudioWriterBase> m_writer;

  double m_performance = 0;
};

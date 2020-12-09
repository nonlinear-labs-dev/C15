#include "AlsaAudioOutput.h"
#include "AudioWriter.h"
#include "AudioEngineOptions.h"
#include "io/HighPriorityTask.h"
#include <nltools/logging/Log.h>
#include <iostream>
#include <algorithm>

int checkAlsa(int res)
{
  if(res != 0)
    nltools::Log::error("Alsa Error:", snd_strerror(res));

  return res;
}

AlsaAudioOutput::AlsaAudioOutput(const AudioEngineOptions* options, const std::string& deviceName, Callback cb)
    : m_cb(cb)
    , m_options(options)
{
  open(deviceName);
}

AlsaAudioOutput::~AlsaAudioOutput()
{
  close();
}

void AlsaAudioOutput::close()
{
  if(auto h = std::exchange(m_handle, nullptr))
    snd_pcm_close(h);
}

void AlsaAudioOutput::open(const std::string& deviceName)
{
  unsigned int sampleRate = static_cast<unsigned int>(m_options->getSampleRate());

  snd_pcm_open(&m_handle, deviceName.c_str(), SND_PCM_STREAM_PLAYBACK, 0);

  snd_pcm_hw_params_t* hwparams = nullptr;
  snd_pcm_sw_params_t* swparams = nullptr;

  snd_pcm_hw_params_alloca(&hwparams);
  snd_pcm_sw_params_alloca(&swparams);

  checkAlsa(snd_pcm_hw_params_any(m_handle, hwparams));
  checkAlsa(snd_pcm_hw_params_set_access(m_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED));

  if(checkAlsa(snd_pcm_hw_params_set_format(m_handle, hwparams, SND_PCM_FORMAT_FLOAT)))
    if(checkAlsa(snd_pcm_hw_params_set_format(m_handle, hwparams, SND_PCM_FORMAT_S32_LE)))
      if(checkAlsa(snd_pcm_hw_params_set_format(m_handle, hwparams, SND_PCM_FORMAT_S16_LE)))
        checkAlsa(snd_pcm_hw_params_set_format(m_handle, hwparams, SND_PCM_FORMAT_S24_3LE));

  snd_pcm_format_t format = SND_PCM_FORMAT_UNKNOWN;
  checkAlsa(snd_pcm_hw_params_get_format(hwparams, &format));
  checkAlsa(snd_pcm_hw_params_set_channels(m_handle, hwparams, 2));
  checkAlsa(snd_pcm_hw_params_set_rate_near(m_handle, hwparams, &sampleRate, nullptr));

  unsigned int periods = static_cast<unsigned int>(m_options->getNumPeriods());
  checkAlsa(snd_pcm_hw_params_set_periods(m_handle, hwparams, periods, 0));
  checkAlsa(snd_pcm_hw_params_get_periods(hwparams, &periods, nullptr));

  snd_pcm_uframes_t framesPerPeriod = static_cast<snd_pcm_uframes_t>(m_options->getFramesPerPeriod());
  checkAlsa(snd_pcm_hw_params_set_period_size_near(m_handle, hwparams, &framesPerPeriod, nullptr));

  unsigned int channels = 0;
  checkAlsa(snd_pcm_hw_params_get_channels(hwparams, &channels));

  snd_pcm_uframes_t ringBufferSize = static_cast<snd_pcm_uframes_t>(m_options->getAlsaRingBufferSize());
  checkAlsa(snd_pcm_hw_params_set_buffer_size_near(m_handle, hwparams, &ringBufferSize));
  checkAlsa(snd_pcm_hw_params(m_handle, hwparams));
  checkAlsa(snd_pcm_sw_params_current(m_handle, swparams));
  checkAlsa(snd_pcm_sw_params(m_handle, swparams));

  nltools::Log::info("Alsa periods:", periods);
  nltools::Log::info("Alsa frames per period:", framesPerPeriod);
  nltools::Log::info("Alsa ringbuffer size:", ringBufferSize);

  m_numFramesPerPeriod = framesPerPeriod;

  m_writer = AudioWriterBase::create(m_handle, format, channels);
}

void AlsaAudioOutput::start()
{
  m_run = true;
  m_task = std::make_unique<HighPriorityTask>(0, [=]() { doBackgroundWork(); });
}

void AlsaAudioOutput::stop()
{
  m_run = false;
  m_task.reset();
}

void AlsaAudioOutput::doBackgroundWork()
{
  pthread_setname_np(pthread_self(), "AudioOut");
  const auto framesPerCallback = m_numFramesPerPeriod;

  snd_pcm_prepare(m_handle);

  SampleFrame prefillAudio[framesPerCallback];
  std::fill(prefillAudio, prefillAudio + framesPerCallback, SampleFrame {});

  snd_pcm_start(m_handle);
  playback(prefillAudio, framesPerCallback);

  SampleFrame audio[framesPerCallback];
  std::fill(audio, audio + framesPerCallback, SampleFrame {});

  auto microsPerBuffer = std::micro::den * framesPerCallback / m_options->getSampleRate();

  while(m_run)
  {
    auto startDSP = g_get_monotonic_time();
    m_cb(audio, framesPerCallback);
    auto endDSP = g_get_monotonic_time();
    playback(audio, framesPerCallback);

    auto diff = endDSP - startDSP;
    reportPerformanceRatio(1.0 * diff / microsPerBuffer);
  }
}

void AlsaAudioOutput::playback(SampleFrame* frames, size_t numFrames)
{
  snd_pcm_sframes_t res = m_writer->write(frames, numFrames);

  if(static_cast<snd_pcm_uframes_t>(res) != numFrames)
    handleWriteError(res);
  else
    m_framesProcessed += numFrames;
}

void AlsaAudioOutput::handleWriteError(snd_pcm_sframes_t result)
{
  if(result < 0)
  {
    if(m_options->areXRunsFatal())
    {
      throw std::runtime_error("XRun");
    }

    if(auto recoverResult = snd_pcm_recover(m_handle, result, 1))
    {
      nltools::Log::error("Could not recover from x-run:", recoverResult);
    }
    else
    {
      nltools::Log::warning("recovered from x-run");
      snd_pcm_start(m_handle);
    }
  }
}

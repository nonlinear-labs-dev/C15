#include "AlsaAudioOutput.h"
#include "AudioWriter.h"
#include "main.h"
#include "Options.h"
#include "io/HighPriorityTask.h"
#include "io/Log.h"
#include <iostream>
#include <algorithm>

int checkAlsa(int res)
{
  if(res != 0)
    Log::error("Alsa Error:", snd_strerror(res));

  return res;
}

AlsaAudioOutput::AlsaAudioOutput(const std::string& deviceName, Callback cb)
    : m_cb(cb)
{
  open(deviceName);
}

AlsaAudioOutput::~AlsaAudioOutput()
{
  close();
}

double AlsaAudioOutput::getPerformance() const
{
  return m_performance * 100;
}

void AlsaAudioOutput::close()
{
  if(auto h = std::exchange(m_handle, nullptr))
    snd_pcm_close(h);
}

void AlsaAudioOutput::open(const std::string& deviceName)
{
  unsigned int sampleRate = static_cast<unsigned int>(getOptions()->getSampleRate());

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

  unsigned int periods = static_cast<unsigned int>(getOptions()->getNumPeriods());
  checkAlsa(snd_pcm_hw_params_set_periods(m_handle, hwparams, periods, 0));
  checkAlsa(snd_pcm_hw_params_get_periods(hwparams, &periods, nullptr));

  snd_pcm_uframes_t framesPerPeriod = static_cast<snd_pcm_uframes_t>(getOptions()->getFramesPerPeriod());
  checkAlsa(snd_pcm_hw_params_set_period_size_near(m_handle, hwparams, &framesPerPeriod, nullptr));

  unsigned int channels = 0;
  checkAlsa(snd_pcm_hw_params_get_channels(hwparams, &channels));

  snd_pcm_uframes_t ringBufferSize = static_cast<snd_pcm_uframes_t>(getOptions()->getAlsaRingBufferSize());
  checkAlsa(snd_pcm_hw_params_set_buffer_size_near(m_handle, hwparams, &ringBufferSize));
  checkAlsa(snd_pcm_hw_params(m_handle, hwparams));
  checkAlsa(snd_pcm_sw_params_current(m_handle, swparams));
  checkAlsa(snd_pcm_sw_params(m_handle, swparams));

  Log::info("Alsa periods:", periods);
  Log::info("Alsa frames per period:", framesPerPeriod);
  Log::info("Alsa ringbuffer size:", ringBufferSize);

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
  const auto framesPerCallback = m_numFramesPerPeriod;

  snd_pcm_prepare(m_handle);

  SampleFrame prefillAudio[framesPerCallback];
  std::fill(prefillAudio, prefillAudio + framesPerCallback, SampleFrame{});

  snd_pcm_start(m_handle);
  playback(prefillAudio, framesPerCallback);

  SampleFrame audio[framesPerCallback];
  std::fill(audio, audio + framesPerCallback, SampleFrame{});

  while(m_run)
  {
    auto startDSP = std::chrono::high_resolution_clock::now();
    m_cb(audio, framesPerCallback);
    auto endDSP = std::chrono::high_resolution_clock::now();
    playback(audio, framesPerCallback);
    auto endPlayback = std::chrono::high_resolution_clock::now();

    auto diffDSP = endDSP - startDSP;
    auto diffAll = endPlayback - startDSP;
    auto ratio = 1.0 * diffDSP / diffAll;
    auto filterCoeff = 0.98;
    m_performance = filterCoeff * m_performance + (1 - filterCoeff) * ratio;
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
    if(getOptions()->areXRunsFatal())
    {
      std::stringstream str;
      str << "XRun at frame position " << m_framesProcessed << std::endl;
      throw std::runtime_error(str.str());
    }

    if(auto recoverResult = snd_pcm_recover(m_handle, result, 1))
    {
      std::cerr << "Could not recover:" << recoverResult << std::endl;
    }
    else
    {
      std::cerr << "recovered from x-run" << std::endl;
      snd_pcm_start(m_handle);
    }
  }
}

#pragma once

#include <Types.h>
#include <array>
#include <memory>
#include <cmath>
#include <alsa/asoundlib.h>

template <snd_pcm_format_t format> struct AlsaFormatToSample;

template <> struct AlsaFormatToSample<SND_PCM_FORMAT_FLOAT>
{
  using Sample = SampleFloat32;
};

template <> struct AlsaFormatToSample<SND_PCM_FORMAT_S32_LE>
{
  using Sample = SampleInt32;
};

template <> struct AlsaFormatToSample<SND_PCM_FORMAT_S16_LE>
{
  using Sample = SampleInt16;
};

class AudioWriterBase
{
 public:
  AudioWriterBase(snd_pcm_t* handle);
  virtual ~AudioWriterBase();

  static std::unique_ptr<AudioWriterBase> create(snd_pcm_t* handle, snd_pcm_format_t format, unsigned int channels);
  template <snd_pcm_format_t format>
  static std::unique_ptr<AudioWriterBase> create(snd_pcm_t* handle, unsigned int channels);

  virtual snd_pcm_sframes_t write(const SampleFrame* frames, size_t numFrames) = 0;

 protected:
  inline static void convertSample(SampleFloat32& out, const Sample in)
  {
    out = in;
  }

  inline static void convertSample(SampleInt16& out, const Sample in)
  {
    out = std::roundf(in * std::numeric_limits<int16_t>::max());
  }

  inline static void convertSample(SampleInt32& out, const Sample in)
  {
    constexpr float f = std::numeric_limits<int32_t>::max() - 64;
    out = std::roundf(in * f);
  }

  template <typename TargetFrame, int channels> snd_pcm_sframes_t write(const SampleFrame* frames, size_t numFrames)
  {
    TargetFrame converted[numFrames];

    for(size_t f = 0; f < numFrames; f++)
    {
      convertSample(converted[f][0], frames[f].left);
      convertSample(converted[f][1], frames[f].right);

      for(size_t c = 2; c < channels; c++)
        converted[f][c] = {};
    }

    return snd_pcm_writei(m_handle, converted, numFrames);
  }

 private:
  snd_pcm_t* m_handle;
};

template <snd_pcm_format_t format, int channels> class AudioWriter : public AudioWriterBase
{
 public:
  using AudioWriterBase::AudioWriterBase;

  snd_pcm_sframes_t write(const SampleFrame* frames, size_t numFrames) override
  {
    using TargetFrame = std::array<typename AlsaFormatToSample<format>::Sample, channels>;
    return AudioWriterBase::write<TargetFrame, channels>(frames, numFrames);
  }
};

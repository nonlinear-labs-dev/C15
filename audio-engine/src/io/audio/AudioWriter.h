#pragma once

#include <Types.h>
#include <array>
#include <memory>

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

template <> struct AlsaFormatToSample<SND_PCM_FORMAT_S24_3LE>
{
  using Sample = SampleInt24;
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
    out = static_cast<SampleInt16>(in * std::numeric_limits<int16_t>::max());
  }

  inline static void convertSample(SampleInt24& out, const Sample in)
  {
    constexpr auto factor = static_cast<float>(1 << 23) - 1;
    int32_t i = static_cast<int32_t>(in * factor);
    memcpy(&out, &i, 3);
  }

  inline static void convertSample(SampleInt32& out, const Sample in)
  {
    out = static_cast<SampleInt32>(in * std::numeric_limits<int32_t>::max());
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

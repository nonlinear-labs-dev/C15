#include "AudioWriter.h"
#include "endian.h"
#include <glibmm.h>

AudioWriterBase::AudioWriterBase(snd_pcm_t *handle)
    : m_handle(handle)
{
#if __BYTE_ORDER != __LITTLE_ENDIAN
#error "platform not supported"
#endif

  {
    Sample max = 1.0f;
    Sample min = -1.0f;

    SampleFloat32 fOutMax;
    SampleFloat32 fOutMin;

    convertSample(fOutMax, max);
    convertSample(fOutMin, min);

    g_assert(max == fOutMax);
    g_assert(min == fOutMin);

    SampleInt16 outI16Max;
    SampleInt16 outI16Min;

    convertSample(outI16Max, max);
    convertSample(outI16Min, min);

    g_assert(outI16Max == std::numeric_limits<int16_t>::max());
    g_assert(outI16Min == -std::numeric_limits<int16_t>::max());

    SampleInt32 outi32Max;
    SampleInt32 outi32Min;

    convertSample(outi32Max, max);
    convertSample(outi32Min, min);

    g_assert(std::abs(outi32Max - std::numeric_limits<int32_t>::max()) < 128);
    g_assert(std::abs(outi32Min + std::numeric_limits<int32_t>::max()) < 128);
  }

  auto expect16 = [](float in, SampleInt16 out) {
    SampleInt16 c;
    convertSample(c, in);
    g_assert(c == out);
  };

  auto expect32 = [](float in, SampleInt32 out) {
    SampleInt32 c;
    convertSample(c, in);
    g_assert(c == out);
  };

  auto e16 = 1.0f / std::pow(2.0f, 15.0f);
  expect16(2 * e16, 2);
  expect16(1.75f * e16, 2);
  expect16(1.25f * e16, 1);
  expect16(1 * e16, 1);
  expect16(0.75f * e16, 1);
  expect16(0.25f * e16, 0);
  expect16(0 * e16, 0);
  expect16(-0.25f * e16, 0);
  expect16(-0.75f * e16, -1);
  expect16(-1 * e16, -1);
  expect16(-1.25f * e16, -1);
  expect16(-1.75f * e16, -2);
  expect16(-2 * e16, -2);

  auto e32 = 1.0f / std::pow(2.0f, 31.0f);
  expect32(2 * e32, 2);
  expect32(1.75f * e32, 2);
  expect32(1.25f * e32, 1);
  expect32(1 * e32, 1);
  expect32(0.75f * e32, 1);
  expect32(0.25f * e32, 0);
  expect32(0 * e32, 0);
  expect32(-0.25f * e32, 0);
  expect32(-0.75f * e32, -1);
  expect32(-1 * e32, -1);
  expect32(-1.25f * e32, -1);
  expect32(-1.75f * e32, -2);
  expect32(-2 * e32, -2);
}

AudioWriterBase::~AudioWriterBase() = default;

std::unique_ptr<AudioWriterBase> AudioWriterBase::create(snd_pcm_t *handle, snd_pcm_format_t format,
                                                         unsigned int channels)
{
  switch(format)
  {
    case SND_PCM_FORMAT_FLOAT:
      return create<SND_PCM_FORMAT_FLOAT>(handle, channels);

    case SND_PCM_FORMAT_S32_LE:
      return create<SND_PCM_FORMAT_S32_LE>(handle, channels);

    case SND_PCM_FORMAT_S16_LE:
      return create<SND_PCM_FORMAT_S16_LE>(handle, channels);

    default:
      break;
  }

  g_return_val_if_reached(nullptr);
}

template <snd_pcm_format_t format>
std::unique_ptr<AudioWriterBase> AudioWriterBase::create(snd_pcm_t *handle, unsigned int channels)
{
  switch(channels)
  {
    case 2:
      return std::make_unique<AudioWriter<format, 2>>(handle);

    case 4:
      return std::make_unique<AudioWriter<format, 4>>(handle);

    case 6:
      return std::make_unique<AudioWriter<format, 6>>(handle);

    case 8:
      return std::make_unique<AudioWriter<format, 8>>(handle);

    case 10:
      return std::make_unique<AudioWriter<format, 10>>(handle);
  }

  g_return_val_if_reached(nullptr);
}

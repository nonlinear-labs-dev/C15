#include "AudioWriter.h"
#include "endian.h"
#include <glibmm.h>

AudioWriterBase::AudioWriterBase(snd_pcm_t *handle)
    : m_handle(handle)
{
#if __BYTE_ORDER != __LITTLE_ENDIAN
#error "platform not supported"
#endif
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

    case SND_PCM_FORMAT_S24_3LE:
      return create<SND_PCM_FORMAT_S24_3LE>(handle, channels);

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

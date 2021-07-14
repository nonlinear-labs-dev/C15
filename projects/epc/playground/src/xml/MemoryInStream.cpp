#include <gio/gio.h>
#include <xml/MemoryInStream.h>
#include <device-settings/DebugLevel.h>

namespace
{
  std::vector<uint8_t> toBuffer(const Glib::ustring &mem)
  {
    auto &raw = mem.raw();
    return { raw.data(), raw.data() + raw.size() };
  }

  std::vector<uint8_t> toBuffer(SoupBuffer *buffer)
  {
    return { buffer->data, buffer->data + buffer->length };
  }
}

MemoryInStream::MemoryInStream(const Glib::ustring &mem, bool zipped)
    : MemoryInStream(toBuffer(mem), zipped)
{
}

MemoryInStream::MemoryInStream(SoupBuffer *buffer, bool zipped)
    : MemoryInStream(toBuffer(buffer), zipped)
{
}

MemoryInStream::MemoryInStream(std::vector<uint8_t> buffer, bool zipped)
    : m_buffer(buffer)
{
  if(GInputStream *memStream = g_memory_input_stream_new_from_data(m_buffer.data(), m_buffer.size(), nullptr))
  {
    if(zipped)
    {
      auto decompressor = g_zlib_decompressor_new(GZlibCompressorFormat::G_ZLIB_COMPRESSOR_FORMAT_GZIP);
      auto converter = g_converter_input_stream_new(G_INPUT_STREAM(memStream), G_CONVERTER(decompressor));
      m_stream = g_data_input_stream_new(G_INPUT_STREAM(converter));
      g_object_unref(converter);
      g_object_unref(decompressor);
    }
    else
    {
      m_stream = g_data_input_stream_new(G_INPUT_STREAM(memStream));
    }

    g_object_unref(memStream);
  }
}

MemoryInStream::~MemoryInStream()
{
  if(m_stream)
    g_object_unref(m_stream);
}

bool MemoryInStream::eof() const
{
  return m_eof;
}

Glib::ustring MemoryInStream::read()
{
  GError *error = nullptr;
  gsize length = 0;
  char *str = g_data_input_stream_read_line(m_stream, &length, nullptr, &error);

  if(error)
  {
    DebugLevel::error(__PRETTY_FUNCTION__, error->message);
    g_error_free(error);
  }

  if(!str)
  {
    m_eof = true;
    return {};
  }

  return Glib::ustring(str) + '\n';
}

std::vector<uint8_t> MemoryInStream::readAll()
{
  std::vector<uint8_t> ret;

  size_t len = 4096;
  uint8_t data[len];
  gsize bytesRead = 0;
  GError *error = nullptr;

  while(g_input_stream_read_all(G_INPUT_STREAM(m_stream), data, len, &bytesRead, nullptr, &error))
  {
    ret.insert(ret.end(), data, data + bytesRead);
  }

  m_eof = true;

  if(error)
  {
    DebugLevel::error(__PRETTY_FUNCTION__, error->message);
    g_error_free(error);
  }

  return ret;
}

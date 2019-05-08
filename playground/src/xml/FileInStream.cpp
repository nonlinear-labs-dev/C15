#include <xml/FileInStream.h>
#include <device-settings/DebugLevel.h>
#include <tools/FileTools.h>
#include <assert.h>

static bool doesZipFileExists(const Glib::ustring &fileName)
{
  return FileTools::doesFileExists(fileName + ".zip");
}

static bool doesGzFileExists(const Glib::ustring &fileName)
{
  return FileTools::doesFileExists(fileName + ".gz");
}

FileInStream::FileInStream(const Glib::ustring &fileName, bool tryZip)
{

  auto file = Gio::File::create_for_path(fileName);

  if(tryZip)
  {
    if(doesZipFileExists(fileName))
    {
      file = Gio::File::create_for_path(fileName + ".zip");
    }
    else if(doesGzFileExists(fileName))
    {
      file = Gio::File::create_for_path(fileName + ".gz");
    }
    else
    {
      tryZip = false;
    }
  }

  if(GFileInputStream *fileStream = g_file_read(file->gobj(), nullptr, nullptr))
  {
    if(tryZip)
    {
      auto decompressor = g_zlib_decompressor_new(GZlibCompressorFormat::G_ZLIB_COMPRESSOR_FORMAT_GZIP);
      auto converter = g_converter_input_stream_new(G_INPUT_STREAM(fileStream), G_CONVERTER(decompressor));
      m_stream = g_data_input_stream_new(G_INPUT_STREAM(converter));
      g_object_unref(converter);
      g_object_unref(decompressor);
    }
    else
    {
      m_stream = g_data_input_stream_new(G_INPUT_STREAM(fileStream));
    }

    g_object_unref(fileStream);
  }

  if(!m_stream)
    m_eof = true;
}

FileInStream::~FileInStream()
{
  if(m_stream)
    g_object_unref(m_stream);
}

ustring FileInStream::read()
{
  if(!m_stream)
  {
    m_eof = true;
    return "";
  }

  try
  {
    ustring str;
    GError *error = nullptr;

    if(char *tmp = g_data_input_stream_read_line_utf8(m_stream, 0, 0, &error))
    {
      str = tmp;
      str += '\n';
      g_free(tmp);
    }

    if(error)
    {
      DebugLevel::error(__PRETTY_FUNCTION__, __LINE__, error->message);
      g_error_free(error);
    }

    m_eof = str.empty();
    return str;
  }
  catch(...)
  {
    DebugLevel::warning("exception on reading file");
    m_eof = true;
    return "";
  }
}

std::vector<uint8_t> FileInStream::readAll()
{
  vector<uint8_t> ret;

  while(m_stream)
  {
    gsize chunkSize = 8192;
    uint8_t data[chunkSize];
    gsize bytesRead = 0;
    GError *error = nullptr;

    g_input_stream_read_all(G_INPUT_STREAM(m_stream), (void *) data, chunkSize, &bytesRead, nullptr, &error);

    if(error)
    {
      DebugLevel::error(__PRETTY_FUNCTION__, __LINE__, error->message);
      g_error_free(error);
    }

    if(!bytesRead)
    {
      break;
    }

    ret.insert(ret.end(), data, data + bytesRead);
  }

  m_eof = true;
  return ret;
}

bool FileInStream::eof() const
{
  return m_eof;
}

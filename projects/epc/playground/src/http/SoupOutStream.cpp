#include "SoupOutStream.h"
#include <giomm.h>

constexpr size_t scratchbufferSize = 1500;  // as suggested by https://de.wikipedia.org/wiki/Maximum_Transmission_Unit

SoupOutStream::SoupOutStream(SoupServer *server, SoupMessage *msg, const Glib::ustring &contentType, bool zip)
    : m_server(server)
    , m_msg(msg)
{
  m_scratchBuffer.reserve(scratchbufferSize);

  soup_message_set_status(msg, SOUP_STATUS_OK);
  soup_message_headers_append(msg->response_headers, "Content-Type", contentType.c_str());
  soup_message_headers_set_encoding(msg->response_headers, SOUP_ENCODING_CHUNKED);
  soup_message_body_set_accumulate(msg->response_body, FALSE);

  if(zip)
    m_zip = Gio::ZlibCompressor::create(Gio::ZLIB_COMPRESSOR_FORMAT_GZIP, 6);
}

void SoupOutStream::flush()
{
  if(!m_completed)
  {
    if(m_zip)
    {
      guint8 outBuf[4096];
      gsize bytes_read = 0;
      gsize bytes_written = 0;
      Gio::ConverterResult res;
      do
      {
        res = m_zip->convert(NULL, 0, outBuf, sizeof(outBuf), Gio::CONVERTER_INPUT_AT_END, bytes_read, bytes_written);
        if(bytes_written)
          push((const char *) (outBuf), bytes_written);

        bytes_written = 0;
      } while(res != Gio::CONVERTER_FINISHED);
    }
    soup_message_body_append(m_msg->response_body, SOUP_MEMORY_COPY, m_scratchBuffer.data(), m_scratchBuffer.size());
    soup_message_body_complete(m_msg->response_body);
    soup_server_unpause_message(m_server, m_msg);
  }
  m_completed = true;
}

SoupOutStream::~SoupOutStream()
{
  flush();
}

void SoupOutStream::push(const Glib::ustring &str)
{
  push(str.c_str(), str.bytes());
}

void SoupOutStream::push(const char *str, size_t numBytes)
{
  if(m_scratchBuffer.size() + numBytes > m_scratchBuffer.capacity())
  {
    if(!m_scratchBuffer.empty())
      soup_message_body_append(m_msg->response_body, SOUP_MEMORY_COPY, m_scratchBuffer.data(), m_scratchBuffer.size());

    soup_message_body_append(m_msg->response_body, SOUP_MEMORY_COPY, str, numBytes);
    soup_server_unpause_message(m_server, m_msg);
    m_scratchBuffer.clear();
  }
  else
  {
    m_scratchBuffer.insert(m_scratchBuffer.end(), str, str + numBytes);
  }
}

void SoupOutStream::implWrite(const Glib::ustring &str)
{
  if(!m_zip)
  {
    push(str);
    return;
  }

  size_t todo = str.raw().size();
  const char *data = str.raw().data();

  while(todo)
  {
    guint8 outBuf[4096];
    gsize bytes_read = 0;
    gsize bytes_written = 0;

    m_zip->convert(data, todo, outBuf, sizeof(outBuf), Gio::CONVERTER_NO_FLAGS, bytes_read, bytes_written);

    todo -= bytes_read;
    data += bytes_read;

    if(bytes_written)
      push((const char *) outBuf, bytes_written);
  }
}

void SoupOutStream::implWrite(const void *buf, size_t numBytes)
{
  push((const char *) buf, numBytes);
}

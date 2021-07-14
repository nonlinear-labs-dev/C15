#include "ZippedMemoryOutStream.h"

ZippedMemoryOutStream::ZippedMemoryOutStream()
{
  m_zip = Gio::ZlibCompressor::create(Gio::ZLIB_COMPRESSOR_FORMAT_GZIP, 6);
}

std::vector<uint8_t> &&ZippedMemoryOutStream::exhaust()
{
  Gio::ConverterResult res;

  do
  {
    guint8 outBuf[4096];
    gsize numBytesRead = 0;
    gsize numBytesWritten = 0;
    res = m_zip->convert(NULL, 0, outBuf, sizeof(outBuf), Gio::CONVERTER_INPUT_AT_END, numBytesRead, numBytesWritten);

    if(numBytesWritten)
      m_buffer.insert(m_buffer.end(), outBuf, outBuf + numBytesWritten);

  } while(res != Gio::CONVERTER_FINISHED);

  return std::move(m_buffer);
}

void ZippedMemoryOutStream::implWrite(const Glib::ustring &str)
{
  implWrite(str.raw().data(), str.raw().size());
}

void ZippedMemoryOutStream::implWrite(const void *buf, size_t numBytes)
{
  auto src = reinterpret_cast<const uint8_t *>(buf);

  while(numBytes)
  {
    guint8 outBuf[4096];
    gsize numBytesRead = 0;
    gsize numBytesWritten = 0;

    m_zip->convert(src, numBytes, outBuf, sizeof(outBuf), Gio::CONVERTER_NO_FLAGS, numBytesRead, numBytesWritten);

    numBytes -= numBytesRead;
    src += numBytesRead;

    if(numBytesWritten)
      m_buffer.insert(m_buffer.end(), outBuf, outBuf + numBytesWritten);
  }
}

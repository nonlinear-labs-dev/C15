#include "MessageComposer.h"
#include <math.h>
#include <string.h>
#include "device-settings/DebugLevel.h"
#include <giomm.h>

MessageComposer::MessageComposer(uint16_t type)
{
  m_stream = Gio::DataOutputStream::create(Gio::MemoryOutputStream::create(NULL, 0, g_realloc, g_free));
  m_stream->set_byte_order(Gio::DATA_STREAM_BYTE_ORDER_LITTLE_ENDIAN);
  m_stream->put_uint16(type);
  m_stream->put_uint16(0);
}

MessageComposer::~MessageComposer()
{
}

void MessageComposer::operator<<(const uint16_t &value)
{
  m_stream->put_uint16(value);
}

void MessageComposer::operator<<(const gint16 &value)
{
  uint16_t v = value;

  if(value < 0)
  {
    v = -value;
    v |= 0x8000;
  }

  *this << v;
}

MessageComposer::tBufferPtr MessageComposer::flush()
{
  m_stream->flush();
  m_stream->close();

  Glib::RefPtr<Gio::MemoryOutputStream> memStream
      = Glib::RefPtr<Gio::MemoryOutputStream>::cast_static(m_stream->get_base_stream());
  size_t numBytes = memStream->get_data_size();
  gconstpointer data = (gconstpointer) memStream->get_data();
  tBufferPtr ret = tBuffer::create(data, numBytes);

  uint16_t numWords = (numBytes - 4) / 2;
  gsize len = 0;
  uint8_t *patchable = (uint8_t *) ret->get_data(len);
  memcpy(patchable + 2, &numWords, 2);

  return ret;
}

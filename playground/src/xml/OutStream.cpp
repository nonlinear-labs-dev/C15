#include "OutStream.h"

OutStream::OutStream()
{
}

OutStream::~OutStream()
{
}

void OutStream::write(const Glib::ustring &str)
{
  implWrite(toUTF8(str));
}

void OutStream::write(const void *buf, size_t numBytes)
{
  implWrite(buf, numBytes);
}

Glib::ustring OutStream::toUTF8(const Glib::ustring &in) const
{
  const gchar *end = NULL;

  if(!g_utf8_validate(in.c_str(), -1, &end))
  {
    gsize bytesWritten = 0;

    gchar *converted
        = g_convert_with_fallback(in.c_str(), -1, "UTF-8", "latin1", (gchar *) "?", NULL, &bytesWritten, NULL);
    Glib::ustring ret = converted;
    g_free(converted);
    return ret;
  }

  return in;
}

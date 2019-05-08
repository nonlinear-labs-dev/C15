#include "Writer.h"
#include <libsoup/soup.h>

Writer::Writer(shared_ptr<OutStream> out)
    : m_out(out)
{
}

Writer::~Writer()
{
}

void Writer::writeToStream(const Glib::ustring &str)
{
  m_out->write(str);
}

void Writer::writeToStream(const char *buf, size_t numBytes)
{
  m_out->write(buf, numBytes);
}

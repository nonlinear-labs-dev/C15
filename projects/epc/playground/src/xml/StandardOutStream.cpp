#include "StandardOutStream.h"

void StandardOutStream::implWrite(const Glib::ustring &str)
{
  *m_out << str;
}

void StandardOutStream::implWrite(const void *buf, size_t numBytes)
{
  implWrite(Glib::ustring(static_cast<const char *>(buf), numBytes));
}

StandardOutStream::StandardOutStream(std::shared_ptr<std::ostream> out)
    : m_out { std::move(out) }
{
}

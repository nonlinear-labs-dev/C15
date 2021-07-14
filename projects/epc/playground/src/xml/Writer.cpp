#include "Writer.h"
#include "Attribute.h"
#include "OutStream.h"

Writer::Writer(OutStream &out)
    : m_out(out)
{
}

Writer::~Writer() = default;

template <typename tFirst, typename... tAttributes>
void Writer::writeAttributes(const tFirst &first, const tAttributes &... attributes)
{
  writeAttributes(first);
  writeAttributes(attributes...);
}

void Writer::writeTextElement(const Glib::ustring &name, const Glib::ustring &text)
{
  implWriteTextElement(name, text, {});
}

void Writer::writeTextElement(const Glib::ustring &name, const Glib::ustring &text, const Attribute &a)
{
  implWriteTextElement(name, text, { a });
}

void Writer::writeTextElement(const Glib::ustring &name, const Glib::ustring &text, const Attribute &a,
                              const Attribute &b)
{
  implWriteTextElement(name, text, { a, b });
}

void Writer::writeTextElement(const Glib::ustring &name, const Glib::ustring &text, const Attribute &a,
                              const Attribute &b, const Attribute &c)
{
  implWriteTextElement(name, text, { a, b, c });
}

void Writer::writeTextElement(const Glib::ustring &name, const Glib::ustring &text, const Attribute &a,
                              const Attribute &b, const Attribute &c, const Attribute &d)
{
  implWriteTextElement(name, text, { a, b, c, d });
}

void Writer::writeTextElement(const Glib::ustring &name, const Glib::ustring &text,
                              std::initializer_list<Attribute> &&a)
{
  implWriteTextElement(name, text, a);
}

void Writer::writeTag(const Glib::ustring &name)
{
  implWriteTag(name, {}, []() {});
}

void Writer::writeTag(const Glib::ustring &name, const Writer::tTagContentWriter &w)
{
  implWriteTag(name, {}, w);
}

void Writer::writeTag(const Glib::ustring &name, std::initializer_list<Attribute> &&a,
                      const Writer::tTagContentWriter &w)
{
  implWriteTag(name, a, w);
}

void Writer::writeTag(const Glib::ustring &name, const Attribute &a, const Writer::tTagContentWriter &w)
{
  implWriteTag(name, { a }, w);
}

void Writer::writeTag(const Glib::ustring &name, const Attribute &a, const Attribute &b,
                      const Writer::tTagContentWriter &w)
{
  implWriteTag(name, { a, b }, w);
}

void Writer::writeTag(const Glib::ustring &name, const Attribute &a, const Attribute &b, const Attribute &c,
                      const Writer::tTagContentWriter &w)
{
  implWriteTag(name, { a, b, c }, w);
}

void Writer::writeTag(const Glib::ustring &name, const Attribute &a, const Attribute &b, const Attribute &c,
                      const Attribute &d, const Writer::tTagContentWriter &w)
{
  implWriteTag(name, { a, b, c, d }, w);
}

void Writer::writeTag(const Glib::ustring &name, const Attribute &a, const Attribute &b, const Attribute &c,
                      const Attribute &d, const Attribute &e, const Writer::tTagContentWriter &w)
{
  implWriteTag(name, { a, b, c, d, e }, w);
}

void Writer::writeTag(const Glib::ustring &name, const Attribute &a, const Attribute &b, const Attribute &c,
                      const Attribute &d, const Attribute &e, const Attribute &f, const Writer::tTagContentWriter &w)
{
  implWriteTag(name, { a, b, c, d, e, f }, w);
}

void Writer::writeTag(const Glib::ustring &name, const Attribute &a, const Attribute &b, const Attribute &c,
                      const Attribute &d, const Attribute &e, const Attribute &f, const Attribute &g,
                      const Writer::tTagContentWriter &w)
{
  implWriteTag(name, { a, b, c, d, e, f, g }, w);
}

void Writer::writeToStream(const Glib::ustring &str)
{
  m_out.write(str);
}

void Writer::writeToStream(const char *buf, size_t numBytes)
{
  m_out.write(buf, numBytes);
}

void Writer::writeAttributes(const std::initializer_list<Attribute> &attr)
{
  for(auto &a : attr)
  {
    writeAttributes(a);
  }
}

void Writer::writeAttributes()
{
}

void Writer::writeAttributes(const Writer::tTagContentWriter &w)
{
}

void Writer::writeAttributes(const Attribute &a)
{
  implWriteAttribute(a);
}

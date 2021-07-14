#include "XmlWriter.h"
#include <libsoup/soup.h>
#include <glibmm.h>

XmlWriter::XmlWriter(OutStream &out)
    : super(out)
    , m_stackSize(0)
{
  writeToStream("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>");
}

XmlWriter::~XmlWriter() = default;

void XmlWriter::implWriteTextElement(const Glib::ustring &name, const Glib::ustring &text,
                                     const std::initializer_list<Attribute> &attributes)
{
  writeToStream(Glib::ustring(m_stackSize, ' '));
  writeToStream("<");

  Glib::ustring escapedName = Glib::Markup::escape_text(name);
  writeToStream(escapedName);
  writeAttributes(attributes);
  writeToStream(">");
  writeToStream(Glib::Markup::escape_text(to_string(text)));
  writeToStream("</");
  writeToStream(escapedName);
  writeToStream(">\n");
}

void XmlWriter::implWriteTag(const Glib::ustring &name, const std::initializer_list<Attribute> &attributes,
                             const tTagContentWriter &w)
{
  writeToStream(Glib::ustring(m_stackSize, ' '));
  writeToStream("<");
  Glib::ustring escapedName = Glib::Markup::escape_text(name);
  writeToStream(escapedName);
  writeAttributes(attributes);
  writeToStream(">\n");
  m_stackSize++;
  w();
  m_stackSize--;
  writeToStream(Glib::ustring(m_stackSize, ' '));
  writeToStream("</");
  writeToStream(escapedName);
  writeToStream(">\n");
}

void XmlWriter::implWriteAttribute(const Attribute &a)
{
  writeToStream(" ");
  writeToStream(a.toXmlString());
}

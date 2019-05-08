#include "Serializer.h"
#include "xml/Writer.h"
#include "xml/Reader.h"
#include "xml/CommitableFileOutStream.h"
#include "xml/VersionAttribute.h"
#include "tools/PerformanceTimer.h"

Serializer::Serializer(const Glib::ustring &tagName)
    : m_tagName(tagName)
{
}

Serializer::~Serializer()
{
}

const Glib::ustring &Serializer::getTagName() const
{
  return m_tagName;
}

void Serializer::write(Writer &writer) const
{
  writer.writeTag(m_tagName, [&]() { writeTagContent(writer); });
}

void Serializer::write(Writer &writer, const Attribute &attr) const
{
  writer.writeTag(m_tagName, attr, [&]() { writeTagContent(writer); });
}

void Serializer::read(Reader &reader) const
{
  reader.preProcess();
  readProlog(reader);
  readTagContent(reader);
}

void Serializer::readProlog(Reader &reader) const
{
}

void Serializer::write(RefPtr<Gio::File> folder, const std::string &name)
{
  try
  {
    auto title = folder->get_path() + "/" + name + ".binary";
    PerformanceTimer timer(title.c_str(), 0);
    auto out = make_shared<CommitableFileOutStream>(title, false);
    BinaryWriter writer(out);
    write(writer, VersionAttribute::get());
    out->commit();
  }
  catch(...)
  {
    DebugLevel::warning("Exception during Serializer::write");
  }
}

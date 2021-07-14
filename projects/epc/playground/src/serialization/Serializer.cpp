#include "Serializer.h"
#include "xml/Writer.h"
#include "xml/Reader.h"
#include "xml/CommitableFileOutStream.h"
#include "xml/VersionAttribute.h"
#include "tools/PerformanceTimer.h"
#include <giomm/file.h>

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

Glib::ustring Serializer::getFilePath(const Glib::RefPtr<Gio::File> &file)
{
  return file->get_path();
}

uint64_t Serializer::getFileModificationTime(const Glib::RefPtr<Gio::File> &file)
{
  auto info = file->query_info(G_FILE_ATTRIBUTE_TIME_CHANGED);
  return info->get_attribute_uint64(G_FILE_ATTRIBUTE_TIME_CHANGED);
}

Glib::RefPtr<Gio::File> Serializer::getChild(const Glib::RefPtr<Gio::File> &file, const std::string &name)
{
  return file->get_child(name);
}

bool Serializer::exists(const Glib::RefPtr<Gio::File> &file)
{
  return file->query_exists();
}

void Serializer::write(Glib::RefPtr<Gio::File> folder, const std::string &name)
{
  try
  {
    auto title = folder->get_path() + "/" + name + ".binary";
    PerformanceTimer timer(title.c_str(), 0);
    CommitableFileOutStream stream(title, false);
    BinaryWriter writer(stream);
    write(writer, VersionAttribute::get());
    stream.commit();
  }
  catch(...)
  {
    DebugLevel::warning("Exception during Serializer::write");
  }
}

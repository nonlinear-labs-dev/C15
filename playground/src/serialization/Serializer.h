#pragma once

#include "playground.h"
#include <libundo/undo/Scope.h>
#include "xml/BinaryReader.h"
#include "xml/BinaryWriter.h"
#include "xml/XmlReader.h"
#include "xml/XmlWriter.h"
#include "xml/FileInStream.h"

class Writer;
class Reader;
class Attribute;

class Serializer
{
 public:
  Serializer(const Glib::ustring &tag);
  virtual ~Serializer();

  void write(RefPtr<Gio::File> folder, const std::string &name);

  const Glib::ustring &getTagName() const;
  void write(Writer &writer) const;
  void write(Writer &writer, const Attribute &attr) const;
  void read(Reader &read) const;

  template <typename tSerializer, typename... tArgs>
  static uint64_t read(UNDO::Scope::tTransactionPtr transaction, RefPtr<Gio::File> folder, const std::string &fileName,
                       tArgs... args)
  {
    auto xmlFile = folder->get_child(fileName);
    auto binFile = folder->get_child(fileName + ".binary");
    auto bingzFile = folder->get_child(fileName + ".binary.gz");
    bool binary = bingzFile->query_exists() || binFile->query_exists();

    if(binary)
    {
      return readFormated<tSerializer, BinaryReader>(transaction, binFile, args...);
    }
    else
    {
      return readFormated<tSerializer, XmlReader>(transaction, xmlFile, args...);
    }
  }

 protected:
  virtual void writeTagContent(Writer &writer) const = 0;

  virtual void readProlog(Reader &reader) const;
  virtual void readTagContent(Reader &reader) const = 0;

 private:
  template <typename tSerializer, typename tReader, typename... tArgs>
  static uint64_t readFormated(UNDO::Scope::tTransactionPtr transaction, Glib::RefPtr<Gio::File> file, tArgs... args)
  {
    FileInStream in(file->get_path(), true);
    if(!in.eof())
    {
      tReader reader(in, transaction);
      reader.template read<tSerializer>(args...);
      auto info = file->query_info(G_FILE_ATTRIBUTE_TIME_CHANGED);
      return info->get_attribute_uint64(G_FILE_ATTRIBUTE_TIME_CHANGED);
    }
    return 0;
  }

  Glib::ustring m_tagName;
};

#pragma once

#include "playground.h"
#include "xml/BinaryReader.h"
#include "xml/BinaryWriter.h"
#include "xml/XmlReader.h"
#include "xml/XmlWriter.h"
#include "xml/FileInStream.h"
#include <giomm/file.h>

class Writer;
class Reader;
class Attribute;

class Serializer
{
 public:
  explicit Serializer(const Glib::ustring &tag);
  virtual ~Serializer();

  void write(Glib::RefPtr<Gio::File> folder, const std::string &name);

  [[nodiscard]] const Glib::ustring &getTagName() const;
  void write(Writer &writer) const;
  void write(Writer &writer, const Attribute &attr) const;
  void read(Reader &read) const;

  template <typename tSerializer, typename... tArgs>
  static uint64_t read(UNDO::Transaction *transaction, Glib::RefPtr<Gio::File> folder, const std::string &fileName,
                       tArgs... args)
  {
    auto xmlFile = getChild(folder, fileName);
    auto binFile = getChild(folder, fileName + ".binary");
    auto bingzFile = getChild(folder, fileName + ".binary.gz");
    bool binary = exists(bingzFile) || exists(binFile);

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
  static Glib::ustring getFilePath(const Glib::RefPtr<Gio::File> &file);
  static uint64_t getFileModificationTime(const Glib::RefPtr<Gio::File> &file);
  static Glib::RefPtr<Gio::File> getChild(const Glib::RefPtr<Gio::File> &file, const std::string &name);
  static bool exists(const Glib::RefPtr<Gio::File> &file);

  template <typename tSerializer, typename tReader, typename... tArgs>
  static uint64_t readFormated(UNDO::Transaction *transaction, Glib::RefPtr<Gio::File> file, tArgs... args)
  {
    FileInStream in(getFilePath(file), true);
    if(!in.eof())
    {
      tReader reader(in, transaction);
      reader.template read<tSerializer>(args...);
      return getFileModificationTime(file);
    }
    return 0;
  }

  Glib::ustring m_tagName;
};

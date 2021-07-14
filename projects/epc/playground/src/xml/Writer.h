#pragma once

#include "playground.h"
#include <functional>

class OutStream;
class Attribute;

class Writer
{
 public:
  explicit Writer(OutStream &out);
  virtual ~Writer();

  typedef std::function<void()> tTagContentWriter;

  void writeTextElement(const Glib::ustring &name, const Glib::ustring &text);
  void writeTextElement(const Glib::ustring &name, const Glib::ustring &text, const Attribute &a);
  void writeTextElement(const Glib::ustring &name, const Glib::ustring &text, const Attribute &a, const Attribute &b);
  void writeTextElement(const Glib::ustring &name, const Glib::ustring &text, const Attribute &a, const Attribute &b,
                        const Attribute &c);
  void writeTextElement(const Glib::ustring &name, const Glib::ustring &text, const Attribute &a, const Attribute &b,
                        const Attribute &c, const Attribute &d);
  void writeTextElement(const Glib::ustring &name, const Glib::ustring &text, std::initializer_list<Attribute> &&a);

  void writeTag(const Glib::ustring &name);
  void writeTag(const Glib::ustring &name, const tTagContentWriter &w);
  void writeTag(const Glib::ustring &name, std::initializer_list<Attribute> &&a, const tTagContentWriter &w);
  void writeTag(const Glib::ustring &name, const Attribute &a, const tTagContentWriter &w);
  void writeTag(const Glib::ustring &name, const Attribute &a, const Attribute &b, const tTagContentWriter &w);
  void writeTag(const Glib::ustring &name, const Attribute &a, const Attribute &b, const Attribute &c,
                const tTagContentWriter &w);
  void writeTag(const Glib::ustring &name, const Attribute &a, const Attribute &b, const Attribute &c,
                const Attribute &d, const tTagContentWriter &w);
  void writeTag(const Glib::ustring &name, const Attribute &a, const Attribute &b, const Attribute &c,
                const Attribute &d, const Attribute &e, const tTagContentWriter &w);
  void writeTag(const Glib::ustring &name, const Attribute &a, const Attribute &b, const Attribute &c,
                const Attribute &d, const Attribute &e, const Attribute &f, const tTagContentWriter &w);
  void writeTag(const Glib::ustring &name, const Attribute &a, const Attribute &b, const Attribute &c,
                const Attribute &d, const Attribute &e, const Attribute &f, const Attribute &g,
                const tTagContentWriter &w);

 protected:
  void writeToStream(const Glib::ustring &str);
  void writeToStream(const char *buf, size_t numBytes);

  void writeAttributes(const std::initializer_list<Attribute> &attr);

 private:
  virtual void implWriteTextElement(const Glib::ustring &name, const Glib::ustring &text,
                                    const std::initializer_list<Attribute> &attributes)
      = 0;
  virtual void implWriteTag(const Glib::ustring &name, const std::initializer_list<Attribute> &attributes,
                            const tTagContentWriter &w)
      = 0;
  virtual void implWriteAttribute(const Attribute &a) = 0;

  void writeAttributes();
  void writeAttributes(const tTagContentWriter &w);
  void writeAttributes(const Attribute &a);

  template <typename tFirst, typename... tAttributes>
  void writeAttributes(const tFirst &first, const tAttributes &... attributes);

  OutStream &m_out;
};

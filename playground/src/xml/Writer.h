#pragma once

#include "playground.h"
#include "OutStream.h"
#include "Compatibility.h"
#include "Attribute.h"
#include <functional>

class Writer
{
 public:
  Writer(shared_ptr<OutStream> out);
  virtual ~Writer();

  typedef function<void()> tTagContentWriter;

  void writeTextElement(const Glib::ustring &name, const Glib::ustring &text)
  {
    implWriteTextElement(name, text, {});
  }

  void writeTextElement(const Glib::ustring &name, const Glib::ustring &text, const Attribute &a)
  {
    implWriteTextElement(name, text, { a });
  }

  void writeTextElement(const Glib::ustring &name, const Glib::ustring &text, const Attribute &a, const Attribute &b)
  {
    implWriteTextElement(name, text, { a, b });
  }

  void writeTextElement(const Glib::ustring &name, const Glib::ustring &text, const Attribute &a, const Attribute &b,
                        const Attribute &c)
  {
    implWriteTextElement(name, text, { a, b, c });
  }

  void writeTextElement(const Glib::ustring &name, const Glib::ustring &text, const Attribute &a, const Attribute &b,
                        const Attribute &c, const Attribute &d)
  {
    implWriteTextElement(name, text, { a, b, c, d });
  }

  void writeTag(const Glib::ustring &name)
  {
    implWriteTag(name, {}, []() {});
  }

  void writeTag(const Glib::ustring &name, const tTagContentWriter &w)
  {
    implWriteTag(name, {}, w);
  }

  void writeTag(const Glib::ustring &name, std::initializer_list<Attribute> &&a, const tTagContentWriter &w)
  {
    implWriteTag(name, a, w);
  }

  void writeTag(const Glib::ustring &name, const Attribute &a, const tTagContentWriter &w)
  {
    implWriteTag(name, { a }, w);
  }

  void writeTag(const Glib::ustring &name, const Attribute &a, const Attribute &b, const tTagContentWriter &w)
  {
    implWriteTag(name, { a, b }, w);
  }

  void writeTag(const Glib::ustring &name, const Attribute &a, const Attribute &b, const Attribute &c,
                const tTagContentWriter &w)
  {
    implWriteTag(name, { a, b, c }, w);
  }

  void writeTag(const Glib::ustring &name, const Attribute &a, const Attribute &b, const Attribute &c,
                const Attribute &d, const tTagContentWriter &w)
  {
    implWriteTag(name, { a, b, c, d }, w);
  }

  void writeTag(const Glib::ustring &name, const Attribute &a, const Attribute &b, const Attribute &c,
                const Attribute &d, const Attribute &e, const tTagContentWriter &w)
  {
    implWriteTag(name, { a, b, c, d, e }, w);
  }

  void writeTag(const Glib::ustring &name, const Attribute &a, const Attribute &b, const Attribute &c,
                const Attribute &d, const Attribute &e, const Attribute &f, const tTagContentWriter &w)
  {
    implWriteTag(name, { a, b, c, d, e, f }, w);
  }

  void writeTag(const Glib::ustring &name, const Attribute &a, const Attribute &b, const Attribute &c,
                const Attribute &d, const Attribute &e, const Attribute &f, const Attribute &g,
                const tTagContentWriter &w)
  {
    implWriteTag(name, { a, b, c, d, e, f, g }, w);
  }

 protected:
  void writeToStream(const Glib::ustring &str);
  void writeToStream(const char *buf, size_t numBytes);

  void writeAttributes(const initializer_list<Attribute> &attr)
  {
    for(auto &a : attr)
    {
      writeAttributes(a);
    }
  }

 private:
  virtual void implWriteTextElement(const Glib::ustring &name, const Glib::ustring &text,
                                    const initializer_list<Attribute> &attributes)
      = 0;
  virtual void implWriteTag(const Glib::ustring &name, const initializer_list<Attribute> &attributes,
                            const tTagContentWriter &w)
      = 0;
  virtual void implWriteAttribute(const Attribute &a) = 0;

  void writeAttributes()
  {
  }

  void writeAttributes(const tTagContentWriter &w)
  {
  }

  void writeAttributes(const Attribute &a)
  {
    implWriteAttribute(a);
  }

  template <typename tFirst, typename... tAttributes>
  void writeAttributes(const tFirst &first, const tAttributes &... attributes)
  {
    writeAttributes(first);
    writeAttributes(attributes...);
  }

  shared_ptr<OutStream> m_out;
};

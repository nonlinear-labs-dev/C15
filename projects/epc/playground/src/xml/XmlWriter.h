#pragma once

#include "playground.h"
#include "XmlClass.h"
#include "OutStream.h"
#include "Writer.h"
#include <functional>

class XmlWriter : public Writer
{
 private:
  typedef Writer super;

 public:
  explicit XmlWriter(OutStream &out);
  ~XmlWriter() override;

 private:
  void implWriteTextElement(const Glib::ustring &name, const Glib::ustring &text,
                            const std::initializer_list<Attribute> &attributes) override;
  void implWriteTag(const Glib::ustring &name, const std::initializer_list<Attribute> &attributes,
                    const tTagContentWriter &w) override;
  void implWriteAttribute(const Attribute &a) override;

 private:
  size_t m_stackSize = 0;
};

#pragma once

#include "playground.h"
#include "XmlClass.h"
#include "OutStream.h"
#include "Compatibility.h"
#include "Writer.h"
#include <functional>

class XmlWriter : public Writer
{
 private:
  typedef Writer super;

 public:
  XmlWriter(shared_ptr<OutStream> out);
  virtual ~XmlWriter();

 private:
  virtual void implWriteTextElement(const Glib::ustring &name, const Glib::ustring &text,
                                    const initializer_list<Attribute> &attributes) override;
  virtual void implWriteTag(const Glib::ustring &name, const initializer_list<Attribute> &attributes,
                            const tTagContentWriter &w) override;
  virtual void implWriteAttribute(const Attribute &a) override;

 private:
  size_t m_stackSize = 0;
};

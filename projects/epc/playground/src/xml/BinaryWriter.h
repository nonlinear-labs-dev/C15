#pragma once

#include <xml/Writer.h>

class BinaryWriter : public Writer
{
 public:
  explicit BinaryWriter(OutStream &out);
  ~BinaryWriter() override;

  void implWriteTextElement(const Glib::ustring &name, const Glib::ustring &text,
                            const std::initializer_list<Attribute> &attributes) override;
  void implWriteTag(const Glib::ustring &name, const std::initializer_list<Attribute> &attributes,
                    const tTagContentWriter &w) override;
  void implWriteAttribute(const Attribute &a) override;

 private:
  void writeRawString(const Glib::ustring &str);
  void writeRawAttributes(const std::initializer_list<Attribute> &attributes);
};

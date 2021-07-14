#pragma once

#include <xml/Reader.h>
#include <xml/Attribute.h>
#include <xml/Attributes.h>

class BinaryReader : public Reader
{
 public:
  BinaryReader(InStream &in, UNDO::Transaction *transaction);
  ~BinaryReader() override;

 private:
  void feed() override;
  void parse();
  void readTextElement();
  void readTagElement();
  Attributes readRawAttributes();
  Attribute readAttribute();
  uint32_t readUInt();
  Glib::ustring readRawString();

  std::vector<uint8_t> m_buffer;
  std::vector<uint8_t>::iterator m_it;
};

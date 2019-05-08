#pragma once

#include "Serializer.h"

class EditBuffer;

class EditBufferSerializer : public Serializer
{
 public:
  EditBufferSerializer(shared_ptr<EditBuffer> editBuffer);
  virtual ~EditBufferSerializer();

  static Glib::ustring getTagName();

 private:
  void writeTagContent(Writer &writer) const;
  void readTagContent(Reader &reader) const;

  shared_ptr<EditBuffer> m_editBuffer;
};

#pragma once

#include "Serializer.h"

class EditBuffer;

class EditBufferSerializer : public Serializer
{
 public:
  explicit EditBufferSerializer(EditBuffer* editBuffer);
  ~EditBufferSerializer() override;

  static Glib::ustring getTagName();

 private:
  void writeTagContent(Writer& writer) const override;
  void readTagContent(Reader& reader) const override;

  EditBuffer* m_editBuffer;
};

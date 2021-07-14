#pragma once
#include "Serializer.h"

class EditBuffer;

class RecallEditBufferSerializer : public Serializer
{
 public:
  explicit RecallEditBufferSerializer(EditBuffer *edit);
  static Glib::ustring getTagName();

 protected:
  void writeTagContent(Writer &writer) const override;
  void readTagContent(Reader &reader) const override;

 protected:
  EditBuffer *m_editBuffer;
};
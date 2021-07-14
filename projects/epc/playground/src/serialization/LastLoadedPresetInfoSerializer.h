#pragma once

#include "Serializer.h"

class EditBuffer;

class LastLoadedPresetInfoSerializer : public Serializer
{
 public:
  explicit LastLoadedPresetInfoSerializer(EditBuffer *eb);
  static Glib::ustring getTagName();

 private:
  void writeTagContent(Writer &writer) const override;
  void readTagContent(Reader &reader) const override;

  EditBuffer *m_editBuffer;
};

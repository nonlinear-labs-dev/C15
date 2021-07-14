#pragma once

#include <serialization/Serializer.h>

class Preset;

class PresetParameterGroupsSerializer : public Serializer
{
 public:
  explicit PresetParameterGroupsSerializer(Preset *preset);

  static Glib::ustring getTagName();

 private:
  void writeTagContent(Writer &writer) const override;
  void readTagContent(Reader &reader) const override;

  Preset *m_preset;
};

#pragma once

#include <serialization/Serializer.h>
#include <nltools/Types.h>

class PresetParameterGroup;

class PresetParameterGroupSerializer : public Serializer
{
 public:
  PresetParameterGroupSerializer(PresetParameterGroup *paramGroup, SoundType type);
  ~PresetParameterGroupSerializer() override;

  static Glib::ustring getTagName();

 private:
  void writeTagContent(Writer &writer) const override;
  void readTagContent(Reader &reader) const override;

  PresetParameterGroup *m_paramGroup = nullptr;
  SoundType m_type;
};

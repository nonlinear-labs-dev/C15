#pragma once

#include <serialization/Serializer.h>
#include <nltools/Types.h>

class PresetParameterGroup;

class PresetParameterGroupSerializer : public Serializer
{
 public:
  PresetParameterGroupSerializer(PresetParameterGroup *paramGroup, SoundType type);
  virtual ~PresetParameterGroupSerializer();

  static Glib::ustring getTagName();

 private:
  void writeTagContent(Writer &writer) const;
  void readTagContent(Reader &reader) const;

  PresetParameterGroup *m_paramGroup = nullptr;
  SoundType m_type;
};

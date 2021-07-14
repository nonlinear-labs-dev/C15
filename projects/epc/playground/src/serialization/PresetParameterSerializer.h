#pragma once

#include <serialization/Serializer.h>
#include <nltools/Types.h>

class PresetParameter;

class PresetParameterSerializer : public Serializer
{
 public:
  PresetParameterSerializer(PresetParameter* param, SoundType type);

  static Glib::ustring getTagName();

 private:
  void writeTagContent(Writer& writer) const override;
  void readTagContent(Reader& reader) const override;

  PresetParameter* m_param = nullptr;
  SoundType m_type;
};

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
  void writeTagContent(Writer& writer) const;
  void readTagContent(Reader& reader) const;

  void tryLoadModulateableParameter(Reader& reader) const;
  void tryLoadMacroControlParameter(Reader& reader) const;
  void tryLoadHardwareSourceParameter(Reader& reader) const;
  void loadModulationAmount(Reader& reader, const Glib::ustring& text) const;

  void tryWriteModulateableParameter(Writer& writer) const;
  void tryWriteMacroControlParameter(Writer& writer) const;
  void tryWriteHardwareSourceParameter(Writer& writer) const;

  PresetParameter* m_param = nullptr;
  SoundType m_type;
};

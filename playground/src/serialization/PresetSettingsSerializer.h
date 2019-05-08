#pragma once

#include "presets/Preset.h"
#include "Serializer.h"

class PresetSettingsSerializer : public Serializer
{
 public:
  PresetSettingsSerializer(Preset *preset);

  static Glib::ustring getTagName();

 private:
  void writeTagContent(Writer &writer) const;
  void readTagContent(Reader &reader) const;

  Preset *m_preset;
};

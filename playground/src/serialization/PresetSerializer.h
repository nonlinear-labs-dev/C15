#pragma once

#include "Serializer.h"

class Preset;

class PresetSerializer : public Serializer
{
 public:
  PresetSerializer(Preset *preset, bool ignoreUUIDs = false);
  virtual ~PresetSerializer();

  static Glib::ustring getTagName();

 private:
  void writeTagContent(Writer &writer) const;
  void readTagContent(Reader &reader) const;

  Preset *m_preset;
  bool m_ignoreUUIDs = false;
};

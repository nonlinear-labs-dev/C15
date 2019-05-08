#pragma once

#include "Serializer.h"

class PresetManager;

class PresetManagerMetadataSerializer : public Serializer
{
 private:
  typedef Serializer super;

 public:
  PresetManagerMetadataSerializer(PresetManager &pm);

  static Glib::ustring getTagName();

 private:
  void writeTagContent(Writer &writer) const;
  void readTagContent(Reader &reader) const;

  PresetManager &m_pm;
};

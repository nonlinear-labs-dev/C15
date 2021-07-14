#pragma once

#include <serialization/Serializer.h>

class PresetManager;

class PresetManagerMetadataSerializer : public Serializer
{
 private:
  typedef Serializer super;

 public:
  explicit PresetManagerMetadataSerializer(PresetManager *pm);

  static Glib::ustring getTagName();

 private:
  void writeTagContent(Writer &writer) const override;
  void readTagContent(Reader &reader) const override;

  PresetManager *m_pm = nullptr;
};

#pragma once

#include <serialization/Serializer.h>

class PresetManager;

class PresetManagerSerializer : public Serializer
{
 public:
  explicit PresetManagerSerializer(PresetManager *pm);
  ~PresetManagerSerializer() override;

  static Glib::ustring getTagName();

 private:
  void writeTagContent(Writer &writer) const override;
  void readTagContent(Reader &reader) const override;

  PresetManager *m_pm = nullptr;
};

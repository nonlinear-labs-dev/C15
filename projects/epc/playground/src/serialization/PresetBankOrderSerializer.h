#pragma once

#include <serialization/Serializer.h>

class PresetManager;

class PresetBankOrderSerializer : public Serializer
{
 public:
  explicit PresetBankOrderSerializer(PresetManager *pm);

  static Glib::ustring getTagName();

 protected:
  void writeTagContent(Writer &writer) const override;
  void readTagContent(Reader &reader) const override;

  PresetManager *m_pm = nullptr;
};

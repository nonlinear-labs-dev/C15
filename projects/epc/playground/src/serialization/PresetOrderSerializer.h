#pragma once

#include <serialization/Serializer.h>

class Bank;

class PresetOrderSerializer : public Serializer
{
 public:
  PresetOrderSerializer(Bank *bank, bool ignoreUUIDs);

  static Glib::ustring getTagName();

 protected:
  void writeTagContent(Writer &writer) const override;
  void readTagContent(Reader &reader) const override;

  Bank *m_bank;
  bool m_ignoreUUIDs;
};

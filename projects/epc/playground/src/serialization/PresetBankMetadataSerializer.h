#pragma once

#include <serialization/Serializer.h>

class Bank;

class PresetBankMetadataSerializer : public Serializer
{
 public:
  explicit PresetBankMetadataSerializer(Bank *bank, bool ignoreUUIDs = false);

  static Glib::ustring getTagName();

 protected:
  void writeTagContent(Writer &writer) const override;
  void readTagContent(Reader &reader) const override;

  Bank *m_bank = nullptr;
  bool m_ignoreUUIDs = false;
};

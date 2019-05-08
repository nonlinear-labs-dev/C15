#pragma once

#include "Serializer.h"

class PresetBank;

class PresetBankMetadataSerializer : public Serializer
{
 public:
  PresetBankMetadataSerializer(shared_ptr<PresetBank> bank, bool ignoreUUIDs = false);

  static Glib::ustring getTagName();

 protected:
  void writeTagContent(Writer &writer) const;
  void readTagContent(Reader &reader) const;

  shared_ptr<PresetBank> m_bank;
  bool m_ignoreUUIDs = false;
};

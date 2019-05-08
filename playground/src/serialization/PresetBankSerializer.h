#pragma once

#include "PresetBankMetadataSerializer.h"

class PresetBank;

class PresetBankSerializer : public PresetBankMetadataSerializer
{
 private:
  typedef PresetBankMetadataSerializer super;

 public:
  PresetBankSerializer(shared_ptr<PresetBank> bank, bool ignoreUUIDs = false);

 private:
  void writeTagContent(Writer &writer) const;
  void readTagContent(Reader &reader) const;
};

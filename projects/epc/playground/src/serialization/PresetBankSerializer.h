#pragma once

#include "PresetBankMetadataSerializer.h"

class Bank;

class PresetBankSerializer : public PresetBankMetadataSerializer
{
 private:
  typedef PresetBankMetadataSerializer super;

 public:
  explicit PresetBankSerializer(Bank *bank, bool ignoreUUIDs = false);

 private:
  void writeTagContent(Writer &writer) const override;
  void readTagContent(Reader &reader) const override;
};

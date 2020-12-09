#pragma once
#include "BooleanSetting.h"

class ExternalMidiEnabledSetting : public BooleanSetting
{
 public:
  explicit ExternalMidiEnabledSetting(UpdateDocumentContributor& s);
  virtual ~ExternalMidiEnabledSetting();

 private:
  void syncExternals(SendReason reason) const override;
};

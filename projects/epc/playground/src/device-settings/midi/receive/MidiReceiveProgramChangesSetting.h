#pragma once
#include <device-settings/BooleanSetting.h>

class MidiReceiveProgramChangesSetting : public BooleanSetting
{
 public:
  explicit MidiReceiveProgramChangesSetting(UpdateDocumentContributor &settings);
};

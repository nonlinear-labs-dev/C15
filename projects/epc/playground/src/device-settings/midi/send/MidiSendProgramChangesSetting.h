#pragma once
#include <device-settings/BooleanSetting.h>

class MidiSendProgramChangesSetting : public BooleanSetting
{
 public:
  explicit MidiSendProgramChangesSetting(UpdateDocumentContributor& s);
};

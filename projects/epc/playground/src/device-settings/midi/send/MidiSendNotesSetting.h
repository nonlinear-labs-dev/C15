#pragma once
#include <device-settings/BooleanSetting.h>

class MidiSendNotesSetting : public BooleanSetting
{
 public:
  explicit MidiSendNotesSetting(UpdateDocumentContributor& s);
};

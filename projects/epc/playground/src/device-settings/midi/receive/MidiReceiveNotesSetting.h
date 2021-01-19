#pragma once
#include <device-settings/BooleanSetting.h>

class MidiReceiveNotesSetting : public BooleanSetting
{
 public:
  explicit MidiReceiveNotesSetting(UpdateDocumentContributor &settings);
};

#pragma once
#include <device-settings/BooleanSetting.h>

class MidiSendControllersSetting : public BooleanSetting
{
 public:
  explicit MidiSendControllersSetting(UpdateDocumentContributor& s);
};

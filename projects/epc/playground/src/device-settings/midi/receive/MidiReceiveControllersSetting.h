#pragma once

#include <device-settings/BooleanSetting.h>
class MidiReceiveControllersSetting : public BooleanSetting
{
 public:
  explicit MidiReceiveControllersSetting(UpdateDocumentContributor& p);
};

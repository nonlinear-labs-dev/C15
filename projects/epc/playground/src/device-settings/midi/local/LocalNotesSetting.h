#pragma once
#include <device-settings/BooleanSetting.h>

class LocalNotesSetting : public BooleanSetting
{
 public:
  explicit LocalNotesSetting(UpdateDocumentContributor& s);
};

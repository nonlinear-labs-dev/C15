#pragma once
#include <device-settings/BooleanSetting.h>

class LocalProgramChangesSetting : public BooleanSetting
{
 public:
  explicit LocalProgramChangesSetting(UpdateDocumentContributor& s);
};

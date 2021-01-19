#pragma once
#include <device-settings/BooleanSetting.h>

class LocalControllersSetting : public BooleanSetting
{
 public:
  explicit LocalControllersSetting(UpdateDocumentContributor& s);
};

#pragma once
#include "BooleanSetting.h"

class GlobalLocalEnableSetting : public BooleanSetting
{
 public:
  explicit GlobalLocalEnableSetting(UpdateDocumentContributor& s);
};

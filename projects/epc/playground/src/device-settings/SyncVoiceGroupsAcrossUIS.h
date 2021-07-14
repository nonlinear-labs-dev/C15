#pragma once
#include <device-settings/BooleanSetting.h>

class SyncVoiceGroupsAcrossUIS : public BooleanSetting
{
 private:
  typedef BooleanSetting super;

 public:
  explicit SyncVoiceGroupsAcrossUIS(UpdateDocumentContributor &settings);
};

#pragma once

#include "ParameterGroup.h"

class VoiceGroupMasterGroup : public ParameterGroup
{
 public:
  VoiceGroupMasterGroup(ParameterGroupSet *parent, VoiceGroup voicegroup);
  void init() override;
};

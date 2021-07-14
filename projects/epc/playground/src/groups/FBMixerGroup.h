#pragma once

#include "ParameterGroup.h"

class FBMixerGroup : public ParameterGroup
{
 public:
  FBMixerGroup(ParameterGroupSet *parent, VoiceGroup voicegroup);
  ~FBMixerGroup() override;

  void init() override;
};

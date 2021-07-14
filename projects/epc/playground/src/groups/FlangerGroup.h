#pragma once

#include "ParameterGroup.h"

class FlangerGroup : public ParameterGroup
{
 public:
  FlangerGroup(ParameterGroupSet *parent, VoiceGroup voicegroup);
  ~FlangerGroup() override;

  void init() override;
};

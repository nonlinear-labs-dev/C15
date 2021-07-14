#pragma once

#include "ParameterGroup.h"

class CombFilterGroup : public ParameterGroup
{
 public:
  CombFilterGroup(ParameterGroupSet *parent, VoiceGroup voicegroup);
  ~CombFilterGroup() override;
  void init() override;
};

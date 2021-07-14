#pragma once

#include "ParameterGroup.h"

class GapFilterGroup : public ParameterGroup
{
 public:
  GapFilterGroup(ParameterGroupSet *parent, VoiceGroup voicegroup);
  ~GapFilterGroup() override;

  void init() override;
};

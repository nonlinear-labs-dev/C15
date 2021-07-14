#pragma once

#include "ParameterGroup.h"

class SVFilterGroup : public ParameterGroup
{
 public:
  SVFilterGroup(ParameterGroupSet *parent, VoiceGroup voicegroup);
  ~SVFilterGroup() override;

  void init() override;
};

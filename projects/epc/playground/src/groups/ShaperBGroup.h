#pragma once

#include "ParameterGroup.h"

class ShaperBGroup : public ParameterGroup
{
 public:
  ShaperBGroup(ParameterGroupSet *parent, VoiceGroup voicegroup);
  ~ShaperBGroup() override;

  void init() override;
};

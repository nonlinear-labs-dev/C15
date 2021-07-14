#pragma once

#include "ParameterGroup.h"

class EchoGroup : public ParameterGroup
{
 public:
  EchoGroup(ParameterGroupSet *parent, VoiceGroup voicegroup);
  ~EchoGroup() override;

  void init() override;
};

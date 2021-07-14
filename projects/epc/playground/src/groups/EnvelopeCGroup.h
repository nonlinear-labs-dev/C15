#pragma once

#include "ParameterGroup.h"

class EnvelopeCGroup : public ParameterGroup
{
 public:
  EnvelopeCGroup(ParameterGroupSet *parent, VoiceGroup voicegroup);
  ~EnvelopeCGroup() override;

  void init() override;
};

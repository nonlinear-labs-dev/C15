#pragma once

#include "ParameterGroup.h"

class OscillatorAGroup : public ParameterGroup
{
 public:
  OscillatorAGroup(ParameterGroupSet *parent, VoiceGroup voicegroup);
  ~OscillatorAGroup() override;

  void init() override;
};

#pragma once

#include "ParameterGroup.h"

class OscillatorBGroup : public ParameterGroup
{
 public:
  OscillatorBGroup(ParameterGroupSet *parent, VoiceGroup voicegroup);
  ~OscillatorBGroup() override;

  void init() override;
};

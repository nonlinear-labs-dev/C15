#pragma once

#include "ParameterGroup.h"

class ShaperAGroup : public ParameterGroup
{
 public:
  ShaperAGroup(ParameterGroupSet *parent, VoiceGroup voicegroup);
  ~ShaperAGroup() override;

  void init() override;
};

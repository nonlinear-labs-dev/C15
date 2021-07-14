#pragma once

#include "ParameterGroup.h"

class EnvelopeBGroup : public ParameterGroup
{
 public:
  EnvelopeBGroup(ParameterGroupSet *parent, VoiceGroup voicegroup);
  ~EnvelopeBGroup() override;

  void init() override;
};

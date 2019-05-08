#pragma once

#include "ParameterGroup.h"

class FBMixerGroup : public ParameterGroup
{
 public:
  FBMixerGroup(ParameterGroupSet *parent);
  virtual ~FBMixerGroup();

  void init();
};

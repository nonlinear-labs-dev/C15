#pragma once

#include "ParameterGroup.h"

class OutputMixerGroup : public ParameterGroup
{
 public:
  OutputMixerGroup(ParameterGroupSet *parent);
  virtual ~OutputMixerGroup();

  void init();
};

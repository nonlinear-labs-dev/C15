#pragma once

#include "ParameterGroup.h"

class OscillatorAGroup : public ParameterGroup
{
 public:
  OscillatorAGroup(ParameterGroupSet *parent);
  virtual ~OscillatorAGroup();

  void init();
};

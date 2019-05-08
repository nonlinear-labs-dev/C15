#pragma once

#include "ParameterGroup.h"

class OscillatorBGroup : public ParameterGroup
{
 public:
  OscillatorBGroup(ParameterGroupSet *parent);
  virtual ~OscillatorBGroup();

  void init();
};

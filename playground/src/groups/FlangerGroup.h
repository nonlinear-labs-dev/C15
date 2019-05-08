#pragma once

#include "ParameterGroup.h"

class FlangerGroup : public ParameterGroup
{
 public:
  FlangerGroup(ParameterGroupSet *parent);
  virtual ~FlangerGroup();

  void init();
};

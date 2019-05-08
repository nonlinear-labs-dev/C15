#pragma once

#include "ParameterGroup.h"

class GapFilterGroup : public ParameterGroup
{
 public:
  GapFilterGroup(ParameterGroupSet *parent);
  virtual ~GapFilterGroup();

  void init();
};

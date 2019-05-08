#pragma once

#include "ParameterGroup.h"

class CombFilterGroup : public ParameterGroup
{
 public:
  CombFilterGroup(ParameterGroupSet *parent);
  virtual ~CombFilterGroup();

  void init();
};

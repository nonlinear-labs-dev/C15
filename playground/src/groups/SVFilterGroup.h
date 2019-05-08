#pragma once

#include "ParameterGroup.h"

class SVFilterGroup : public ParameterGroup
{
 public:
  SVFilterGroup(ParameterGroupSet *parent);
  virtual ~SVFilterGroup();

  void init();
};

#pragma once

#include "ParameterGroup.h"

class CabinetGroup : public ParameterGroup
{
 public:
  CabinetGroup(ParameterGroupSet *parent);
  virtual ~CabinetGroup();

  void init();
};

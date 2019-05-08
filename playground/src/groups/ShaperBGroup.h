#pragma once

#include "ParameterGroup.h"

class ShaperBGroup : public ParameterGroup
{
 public:
  ShaperBGroup(ParameterGroupSet *parent);
  virtual ~ShaperBGroup();

  void init();
};

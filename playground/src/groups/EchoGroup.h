#pragma once

#include "ParameterGroup.h"

class EchoGroup : public ParameterGroup
{
 public:
  EchoGroup(ParameterGroupSet *parent);
  virtual ~EchoGroup();

  void init();
};

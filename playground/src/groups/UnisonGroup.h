#pragma once

#include "ParameterGroup.h"

class UnisonGroup : public ParameterGroup
{
 public:
  UnisonGroup(ParameterGroupSet *parent);
  virtual ~UnisonGroup();

  void init();
};

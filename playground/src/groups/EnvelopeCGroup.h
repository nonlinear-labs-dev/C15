#pragma once

#include "ParameterGroup.h"

class EnvelopeCGroup : public ParameterGroup
{
 public:
  EnvelopeCGroup(ParameterGroupSet *parent);
  virtual ~EnvelopeCGroup();

  void init();
};

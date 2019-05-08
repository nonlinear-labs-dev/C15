#pragma once

#include "ParameterGroup.h"

class EnvelopeBGroup : public ParameterGroup
{
 public:
  EnvelopeBGroup(ParameterGroupSet *parent);
  virtual ~EnvelopeBGroup();

  void init();
};

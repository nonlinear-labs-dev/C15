#pragma once

#include "ParameterGroup.h"

class ShaperAGroup : public ParameterGroup
{
 public:
  ShaperAGroup(ParameterGroupSet *parent);
  virtual ~ShaperAGroup();

  void init();
};

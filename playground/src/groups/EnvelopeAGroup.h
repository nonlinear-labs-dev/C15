#pragma once

#include "ParameterGroup.h"

class EnvelopeAGroup : public ParameterGroup
{
 public:
  EnvelopeAGroup(ParameterGroupSet *parent);
  virtual ~EnvelopeAGroup();

  void init();
};

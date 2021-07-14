#pragma once
#include "ParameterGroup.h"

class SplitParameterGroups : public ParameterGroup
{
 public:
  SplitParameterGroups(ParameterGroupSet* parent, VoiceGroup vg);
  void init() override;

  static bool isSplitPoint(const Parameter* p);
  static bool isSplitPoint(const ParameterId& id);
};

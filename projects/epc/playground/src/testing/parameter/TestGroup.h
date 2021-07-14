#pragma once

#include <groups/ParameterGroup.h>
#include "TestGroupSet.h"

class TestGroup : public ParameterGroup
{
 public:
  explicit TestGroup(TestGroupSet *root, VoiceGroup vg);
  void init() override;
  void writeDocument(Writer &writer, tUpdateID knownRevision) const override;
  void addParameter(Parameter *p);
};
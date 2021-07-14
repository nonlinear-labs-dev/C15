#pragma once

#include "ParameterGroup.h"

class UnisonGroup : public ParameterGroup
{
 public:
  UnisonGroup(ParameterGroupSet *parent, VoiceGroup voicegroup);
  ~UnisonGroup() override;

  void init() override;

  static bool isUnisonParameter(const Parameter *parameter);
  static bool isUnisonParameter(const ParameterId &id);

  static bool isUnisonVoicesParameter(const Parameter *parameter);
  static bool isUnisonVoicesParameter(const ParameterId &id);
};

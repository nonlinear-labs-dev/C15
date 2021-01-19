#pragma once
#include <device-settings/VelocityCurve.h>
#include <device-settings/NLEnumSetting.h>

class MidiReceiveVelocityCurveSetting : public NLEnumSetting<VelocityCurves>
{
 public:
  explicit MidiReceiveVelocityCurveSetting(UpdateDocumentContributor& s);
};

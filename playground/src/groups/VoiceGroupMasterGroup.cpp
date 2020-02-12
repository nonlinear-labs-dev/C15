#include <parameters/scale-converters/ParabolicGainDbScaleConverter.h>
#include <parameters/scale-converters/LinearBipolar48StScaleConverter.h>
#include <parameters/voice-group-master-group/VoiceGroupMasterParameter.h>
#include <parameters/scale-converters/Linear100PercentScaleConverter.h>
#include <parameters/voice-group-master-group/VoiceGroupMasterParameterWithUnusualModUnit.h>
#include <parameters/scale-converters/LinearBipolar96StScaleConverter.h>
#include "VoiceGroupMasterGroup.h"

VoiceGroupMasterGroup::VoiceGroupMasterGroup(ParameterDualGroupSet *parent, VoiceGroup vg)
    : ParameterGroup(parent, { "Part", vg }, "Part", "Part", "Part")
{
}

void VoiceGroupMasterGroup::init()
{
  appendParameter(new VoiceGroupMasterParameter(this, { 358, getVoiceGroup() },
                                                ScaleConverter::get<ParabolicGainDbScaleConverter>(), 0.5, 100, 1000,
                                                "Volume", "Part Volume", getVoiceGroup()));

  appendParameter(new VoiceGroupMasterParameterWithUnusualModUnit(
      this, { 360, getVoiceGroup() }, ScaleConverter::get<LinearBipolar48StScaleConverter>(),
      ScaleConverter::get<LinearBipolar96StScaleConverter>(), 0, 48, 4800, "Part Tune", "Tune"));
}

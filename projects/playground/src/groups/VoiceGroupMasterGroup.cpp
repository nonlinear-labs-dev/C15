#include <parameters/scale-converters/ParabolicGainDbScaleConverter.h>
#include <parameters/scale-converters/LinearBipolar48StScaleConverter.h>
#include <parameters/voice-group-master-group/VoiceGroupMasterModulateableParameter.h>
#include <parameters/voice-group-master-group/VoiceGroupMasterModulateableParameterWithUnusualModUnit.h>
#include <parameters/scale-converters/OnOffScaleConverter.h>
#include <parameters/scale-converters/Linear60KeyScaleConverter.h>
#include <parameters/scale-converters/dimension/PitchDimension.h>
#include <parameters/scale-converters/Linear60StScaleConverter.h>
#include <parameters/scale-converters/LinearBipolar96StScaleConverterFine.h>
#include "VoiceGroupMasterGroup.h"

VoiceGroupMasterGroup::VoiceGroupMasterGroup(ParameterDualGroupSet *parent, VoiceGroup vg)
    : ParameterGroup(parent, { "Part", vg }, "Part", "Part", "Part")
{
}

void VoiceGroupMasterGroup::init()
{
  appendParameter(new VoiceGroupMasterModulateableParameter(
      this, { 358, getVoiceGroup() }, ScaleConverter::get<ParabolicGainDbScaleConverter>(), 0.5, 100, 1000));

  appendParameter(new VoiceGroupMasterModulateableParameterWithUnusualModUnit(
      this, { 360, getVoiceGroup() }, ScaleConverter::get<LinearBipolar48StScaleConverter>(),
      ScaleConverter::get<LinearBipolar96StScaleConverterFine>(), 0, 48, 4800));

  appendParameter(new VoiceGroupMasterUnmodulateableParameter(this, { 395, getVoiceGroup() },
                                                              ScaleConverter::get<OnOffScaleConverter>(), 0.0, 1, 1));

  auto fadeFromInitial = getVoiceGroup() == VoiceGroup::I ? 1 : 0;

  appendParameter(new VoiceGroupMasterUnmodulateableParameter(
      this, { 396, getVoiceGroup() }, ScaleConverter::get<Linear60KeyScaleConverter>(), fadeFromInitial, 60, 60));

  appendParameter(new VoiceGroupMasterUnmodulateableParameter(
      this, { 397, getVoiceGroup() }, ScaleConverter::get<Linear60StScaleConverter>(), 0.0, 60, 60));
}

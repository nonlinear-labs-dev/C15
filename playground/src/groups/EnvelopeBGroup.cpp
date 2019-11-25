#include <groups/EnvelopeBGroup.h>
#include <parameters/ModulateableParameterWithUnusualModUnit.h>
#include <parameters/scale-converters/EnvelopeAttackDecayTimeMSScaleConverter.h>
#include <parameters/scale-converters/EnvelopeReleaseTimeMSScaleConverter.h>
#include <parameters/scale-converters/EnvelopeTimeMSScaleConverter.h>
#include <parameters/scale-converters/Linear100PercentScaleConverter.h>
#include <parameters/scale-converters/Linear60DbScaleConverter.h>
#include <parameters/scale-converters/Linear60DbtScaleConverter.h>
#include <parameters/scale-converters/LinearBipolar100PercentScaleConverter.h>
#include <parameters/scale-converters/LinearBipolar1DbstScaleConverter.h>
#include <parameters/scale-converters/LinearBipolar24DbScaleConverter.h>
#include <parameters/scale-converters/LinearBipolar48DbScaleConverter.h>
#include <parameters/scale-converters/ScaleConverter.h>
#include <parameters/ModulateableParameterWithUnusualModDenominator.h>

EnvelopeBGroup::EnvelopeBGroup(ParameterDualGroupSet *parent, VoiceGroup voicegroup)
    : ParameterGroup(parent, "Env B", "Envelope B", "Envelope B", "Envelope B", voicegroup)
{
}

EnvelopeBGroup::~EnvelopeBGroup()
{
}

void EnvelopeBGroup::init()
{
  appendParameter(new ModulateableParameter(
      this, { 19, getVoiceGroup() }, ScaleConverter::get<EnvelopeAttackDecayTimeMSScaleConverter>(), 0, 100, 1000));

  appendParameter(new Parameter(this, { 295, getVoiceGroup() },
                                ScaleConverter::get<LinearBipolar100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameter(
      this, { 21, getVoiceGroup() }, ScaleConverter::get<EnvelopeAttackDecayTimeMSScaleConverter>(), 0.59, 100, 1000));

  appendParameter(new ModulateableParameter(this, { 23, getVoiceGroup() },
                                            ScaleConverter::get<Linear100PercentScaleConverter>(), 0.5, 100, 1000));

  appendParameter(new ModulateableParameter(
      this, { 25, getVoiceGroup() }, ScaleConverter::get<EnvelopeAttackDecayTimeMSScaleConverter>(), 0.79, 100, 1000));

  appendParameter(new ModulateableParameter(this, { 27, getVoiceGroup() },
                                            ScaleConverter::get<Linear100PercentScaleConverter>(), 0, 100, 1000));

  appendParameter(new ModulateableParameterWithUnusualModDenominator(
      this, { 29, getVoiceGroup() }, ScaleConverter::get<EnvelopeReleaseTimeMSScaleConverter>(), 0.53, 101, 1010, 100,
      1000));

  appendParameter(new ModulateableParameterWithUnusualModUnit(
      this, { 31, getVoiceGroup() }, ScaleConverter::get<LinearBipolar24DbScaleConverter>(),
      ScaleConverter::get<LinearBipolar48DbScaleConverter>(), 0, 48, 480));

  appendParameter(
      new Parameter(this, { 33, getVoiceGroup() }, ScaleConverter::get<Linear60DbScaleConverter>(), 0.5, 60, 600));

  appendParameter(
      new Parameter(this, { 34, getVoiceGroup() }, ScaleConverter::get<Linear60DbtScaleConverter>(), 0, 60, 600));

  appendParameter(
      new Parameter(this, { 35, getVoiceGroup() }, ScaleConverter::get<Linear60DbtScaleConverter>(), 0, 60, 600));

  appendParameter(new Parameter(this, { 36, getVoiceGroup() }, ScaleConverter::get<LinearBipolar1DbstScaleConverter>(),
                                0, 100, 1000));

  appendParameter(new Parameter(this, { 37, getVoiceGroup() }, ScaleConverter::get<Linear100PercentScaleConverter>(),
                                0.05, 100, 1000));
}

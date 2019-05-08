#include "MacroControlsGroup.h"
#include "parameters/MacroControlParameter.h"
#include "parameters/scale-converters/EnvelopeAttackDecayTimeMSScaleConverter.h"

MacroControlsGroup::MacroControlsGroup(ParameterGroupSet *parent)
    : ParameterGroup(parent, "MCs", "Macro Control", "Macro Control", "Macro Control")
{
}

MacroControlsGroup::~MacroControlsGroup()
{
}

void MacroControlsGroup::init()
{
  appendParameter(new MacroControlParameter(this, 243));
  appendParameter(new MacroControlParameter(this, 244));
  appendParameter(new MacroControlParameter(this, 245));
  appendParameter(new MacroControlParameter(this, 246));

  appendParameter(
      new Parameter(this, 324, ScaleConverter::get<EnvelopeAttackDecayTimeMSScaleConverter>(), 0.4, 100, 1000));
  appendParameter(
      new Parameter(this, 325, ScaleConverter::get<EnvelopeAttackDecayTimeMSScaleConverter>(), 0.4, 100, 1000));
  appendParameter(
      new Parameter(this, 326, ScaleConverter::get<EnvelopeAttackDecayTimeMSScaleConverter>(), 0.4, 100, 1000));
  appendParameter(
      new Parameter(this, 327, ScaleConverter::get<EnvelopeAttackDecayTimeMSScaleConverter>(), 0.4, 100, 1000));
}

uint16_t MacroControlsGroup::modSrcToParamID(ModulateableParameter::ModulationSource src)
{
  switch(src)
  {
    case ModulateableParameter::MC1:
      return 243;

    case ModulateableParameter::MC2:
      return 244;

    case ModulateableParameter::MC3:
      return 245;

    case ModulateableParameter::MC4:
      return 246;

    default:
      return -1;
  }
}

ModulateableParameter::ModulationSource MacroControlsGroup::paramIDToModSrc(uint16_t pid)
{
  switch(pid)
  {
    case 243:
      return ModulateableParameter::MC1;

    case 244:
      return ModulateableParameter::MC2;

    case 245:
      return ModulateableParameter::MC3;

    case 246:
      return ModulateableParameter::MC4;

    default:
      return ModulateableParameter::NONE;
  }
}

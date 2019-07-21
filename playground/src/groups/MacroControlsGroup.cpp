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

uint16_t MacroControlsGroup::modSrcToParamID(MacroControls src)
{
  switch(src)
  {
    case MacroControls::MC1:
      return 243;

    case MacroControls::MC2:
      return 244;

    case MacroControls::MC3:
      return 245;

    case MacroControls::MC4:
      return 246;

    default:
      return -1;
  }
}

MacroControls MacroControlsGroup::paramIDToModSrc(uint16_t pid)
{
  switch(pid)
  {
    case 243:
      return MacroControls::MC1;

    case 244:
      return MacroControls::MC2;

    case 245:
      return MacroControls::MC3;

    case 246:
      return MacroControls::MC4;

    default:
      return MacroControls::NONE;
  }
}

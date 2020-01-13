#include "MacroControlsGroup.h"
#include "parameters/MacroControlParameter.h"
#include "parameters/scale-converters/EnvelopeAttackDecayTimeMSScaleConverter.h"

MacroControlsGroup::MacroControlsGroup(ParameterDualGroupSet *parent)
    : ParameterGroup(parent, { "MCs", VoiceGroup::Global }, "Macro Control", "Macro Control", "Macro Control")
{
}

MacroControlsGroup::~MacroControlsGroup() = default;

void MacroControlsGroup::init()
{

  appendParameter(new MacroControlParameter(this, { 243, VoiceGroup::Global }));
  appendParameter(new MacroControlParameter(this, { 244, VoiceGroup::Global }));
  appendParameter(new MacroControlParameter(this, { 245, VoiceGroup::Global }));
  appendParameter(new MacroControlParameter(this, { 246, VoiceGroup::Global }));
  appendParameter(new MacroControlParameter(this, { 369, VoiceGroup::Global }));
  appendParameter(new MacroControlParameter(this, { 371, VoiceGroup::Global }));

  appendParameter(new Parameter(this, { 324, VoiceGroup::Global },
                                ScaleConverter::get<EnvelopeAttackDecayTimeMSScaleConverter>(), 0.4, 100, 1000));
  appendParameter(new Parameter(this, { 325, VoiceGroup::Global },
                                ScaleConverter::get<EnvelopeAttackDecayTimeMSScaleConverter>(), 0.4, 100, 1000));
  appendParameter(new Parameter(this, { 326, VoiceGroup::Global },
                                ScaleConverter::get<EnvelopeAttackDecayTimeMSScaleConverter>(), 0.4, 100, 1000));
  appendParameter(new Parameter(this, { 327, VoiceGroup::Global },
                                ScaleConverter::get<EnvelopeAttackDecayTimeMSScaleConverter>(), 0.4, 100, 1000));
  appendParameter(new Parameter(this, { 370, VoiceGroup::Global },
                                ScaleConverter::get<EnvelopeAttackDecayTimeMSScaleConverter>(), 0.4, 100, 1000));
  appendParameter(new Parameter(this, { 372, VoiceGroup::Global },
                                ScaleConverter::get<EnvelopeAttackDecayTimeMSScaleConverter>(), 0.4, 100, 1000));
}

ParameterId MacroControlsGroup::modSrcToSmoothingId(MacroControls mcId)
{
  switch(mcId)
  {
    case MacroControls::MC1:
      return ParameterId(324, VoiceGroup::Global);

    case MacroControls::MC2:
      return ParameterId(325, VoiceGroup::Global);

    case MacroControls::MC3:
      return ParameterId(326, VoiceGroup::Global);

    case MacroControls::MC4:
      return ParameterId(327, VoiceGroup::Global);

    case MacroControls::MC5:
      return ParameterId(370, VoiceGroup::Global);

    case MacroControls::MC6:
      return ParameterId(372, VoiceGroup::Global);

    default:
      break;
  }

  return ParameterId::invalid();
}

ParameterId MacroControlsGroup::modSrcToParamId(MacroControls src)
{
  switch(src)
  {
    case MacroControls::MC1:
      return ParameterId(243, VoiceGroup::Global);

    case MacroControls::MC2:
      return ParameterId(244, VoiceGroup::Global);

    case MacroControls::MC3:
      return ParameterId(245, VoiceGroup::Global);

    case MacroControls::MC4:
      return ParameterId(246, VoiceGroup::Global);

    case MacroControls::MC5:
      return ParameterId(369, VoiceGroup::Global);

    case MacroControls::MC6:
      return ParameterId(371, VoiceGroup::Global);

    default:
      break;
  }

  return ParameterId::invalid();
}

MacroControls MacroControlsGroup::paramIDToModSrc(ParameterId pid)
{
  switch(pid.getNumber())
  {
    case 243:
      return MacroControls::MC1;

    case 244:
      return MacroControls::MC2;

    case 245:
      return MacroControls::MC3;

    case 246:
      return MacroControls::MC4;

    case 369:
      return MacroControls::MC5;

    case 371:
      return MacroControls::MC6;

    default:
      return MacroControls::NONE;
  }
}

bool MacroControlsGroup::isMacroTime(const ParameterId &id)
{
  return (id.getNumber() >= 324 && id.getNumber() <= 327) || (id.getNumber() == 370 || id.getNumber() == 372);
}

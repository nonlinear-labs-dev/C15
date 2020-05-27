#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterLayout.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <Application.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/MacroControlSmoothingParameterLayout.h>
#include <parameters/MacroControlSmoothingParameter.h>
#include <parameter_declarations.h>
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

  appendParameter(new MacroControlParameter(this, { C15::PID::MC_A, VoiceGroup::Global }));
  appendParameter(new MacroControlParameter(this, { C15::PID::MC_B, VoiceGroup::Global }));
  appendParameter(new MacroControlParameter(this, { C15::PID::MC_C, VoiceGroup::Global }));
  appendParameter(new MacroControlParameter(this, { C15::PID::MC_D, VoiceGroup::Global }));
  appendParameter(new MacroControlParameter(this, { C15::PID::MC_E, VoiceGroup::Global }));
  appendParameter(new MacroControlParameter(this, { C15::PID::MC_F, VoiceGroup::Global }));

  appendParameter(new MacroControlSmoothingParameter(this, { C15::PID::MC_Time_A, VoiceGroup::Global },
                                                     ScaleConverter::get<EnvelopeAttackDecayTimeMSScaleConverter>(),
                                                     0.442, 100, 1000));
  appendParameter(new MacroControlSmoothingParameter(this, { C15::PID::MC_Time_B, VoiceGroup::Global },
                                                     ScaleConverter::get<EnvelopeAttackDecayTimeMSScaleConverter>(),
                                                     0.442, 100, 1000));
  appendParameter(new MacroControlSmoothingParameter(this, { C15::PID::MC_Time_C, VoiceGroup::Global },
                                                     ScaleConverter::get<EnvelopeAttackDecayTimeMSScaleConverter>(),
                                                     0.442, 100, 1000));
  appendParameter(new MacroControlSmoothingParameter(this, { C15::PID::MC_Time_D, VoiceGroup::Global },
                                                     ScaleConverter::get<EnvelopeAttackDecayTimeMSScaleConverter>(),
                                                     0.442, 100, 1000));
  appendParameter(new MacroControlSmoothingParameter(this, { C15::PID::MC_Time_E, VoiceGroup::Global },
                                                     ScaleConverter::get<EnvelopeAttackDecayTimeMSScaleConverter>(),
                                                     0.442, 100, 1000));
  appendParameter(new MacroControlSmoothingParameter(this, { C15::PID::MC_Time_F, VoiceGroup::Global },
                                                     ScaleConverter::get<EnvelopeAttackDecayTimeMSScaleConverter>(),
                                                     0.442, 100, 1000));
}

ParameterId MacroControlsGroup::modSrcToSmoothingId(MacroControls mcId)
{
  switch(mcId)
  {
    case MacroControls::MC1:
      return ParameterId(C15::PID::MC_Time_A, VoiceGroup::Global);

    case MacroControls::MC2:
      return ParameterId(C15::PID::MC_Time_B, VoiceGroup::Global);

    case MacroControls::MC3:
      return ParameterId(C15::PID::MC_Time_C, VoiceGroup::Global);

    case MacroControls::MC4:
      return ParameterId(C15::PID::MC_Time_D, VoiceGroup::Global);

    case MacroControls::MC5:
      return ParameterId(C15::PID::MC_Time_E, VoiceGroup::Global);

    case MacroControls::MC6:
      return ParameterId(C15::PID::MC_Time_F, VoiceGroup::Global);

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
      return ParameterId(C15::PID::MC_A, VoiceGroup::Global);

    case MacroControls::MC2:
      return ParameterId(C15::PID::MC_B, VoiceGroup::Global);

    case MacroControls::MC3:
      return ParameterId(C15::PID::MC_C, VoiceGroup::Global);

    case MacroControls::MC4:
      return ParameterId(C15::PID::MC_D, VoiceGroup::Global);

    case MacroControls::MC5:
      return ParameterId(C15::PID::MC_E, VoiceGroup::Global);

    case MacroControls::MC6:
      return ParameterId(C15::PID::MC_F, VoiceGroup::Global);

    default:
      break;
  }

  return ParameterId::invalid();
}

MacroControls MacroControlsGroup::paramIDToModSrc(const ParameterId &pid)
{
  switch(pid.getNumber())
  {
    case C15::PID::MC_A:
      return MacroControls::MC1;

    case C15::PID::MC_B:
      return MacroControls::MC2;

    case C15::PID::MC_C:
      return MacroControls::MC3;

    case C15::PID::MC_D:
      return MacroControls::MC4;

    case C15::PID::MC_E:
      return MacroControls::MC5;

    case C15::PID::MC_F:
      return MacroControls::MC6;

    default:
      return MacroControls::NONE;
  }
}

bool MacroControlsGroup::isMacroTime(const ParameterId &id)
{
  return (id.getNumber() >= C15::PID::MC_Time_A && id.getNumber() <= C15::PID::MC_Time_D)
      || (id.getNumber() == C15::PID::MC_Time_E || id.getNumber() == C15::PID::MC_Time_F);
}

bool MacroControlsGroup::isMacroControl(const int paramNumber)
{
  return paramNumber == C15::PID::MC_A || paramNumber == C15::PID::MC_B || paramNumber == C15::PID::MC_C
      || paramNumber == C15::PID::MC_D || paramNumber == C15::PID::MC_E || paramNumber == C15::PID::MC_F;
}

ParameterId MacroControlsGroup::smoothingIdToMCId(const ParameterId &smoothingId)
{
  switch(smoothingId.getNumber())
  {
    case C15::PID::MC_Time_A:
      return { C15::PID::MC_A, VoiceGroup::Global };
    case C15::PID::MC_Time_B:
      return { C15::PID::MC_B, VoiceGroup::Global };
    case C15::PID::MC_Time_C:
      return { C15::PID::MC_C, VoiceGroup::Global };
    case C15::PID::MC_Time_D:
      return { C15::PID::MC_D, VoiceGroup::Global };
    case C15::PID::MC_Time_E:
      return { C15::PID::MC_E, VoiceGroup::Global };
    case C15::PID::MC_Time_F:
      return { C15::PID::MC_F, VoiceGroup::Global };
    default:
      return ParameterId::invalid();
  }
}

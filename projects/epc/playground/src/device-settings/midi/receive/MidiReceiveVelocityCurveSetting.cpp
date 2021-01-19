#include "MidiReceiveVelocityCurveSetting.h"

MidiReceiveVelocityCurveSetting::MidiReceiveVelocityCurveSetting(UpdateDocumentContributor& s)
    : NLEnumSetting<VelocityCurves>(s, VelocityCurves::VELOCITY_CURVE_NORMAL)
{
}

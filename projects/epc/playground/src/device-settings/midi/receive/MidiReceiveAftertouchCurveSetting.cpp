#include "MidiReceiveAftertouchCurveSetting.h"

MidiReceiveAftertouchCurveSetting::MidiReceiveAftertouchCurveSetting(UpdateDocumentContributor& s)
    : NLEnumSetting<AftertouchCurves>(s, AftertouchCurves::AFTERTOUCH_CURVE_NORMAL)
{
}

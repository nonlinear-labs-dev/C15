#pragma once
#include <device-settings/NLEnumSetting.h>
#include <nltools/Types.h>

class MidiReceiveAftertouchCurveSetting : public NLEnumSetting<AftertouchCurves>
{
 public:
  explicit MidiReceiveAftertouchCurveSetting(UpdateDocumentContributor& s);
};

#pragma once
#include <device-settings/EnumSetting.h>
#include <nltools/Types.h>

class MidiReceiveAftertouchCurveSetting : public EnumSetting<AftertouchCurves>
{
 public:
  explicit MidiReceiveAftertouchCurveSetting(UpdateDocumentContributor& s);
  const std::vector<Glib::ustring>& enumToString() const override;
  const std::vector<Glib::ustring>& enumToDisplayString() const override;
};

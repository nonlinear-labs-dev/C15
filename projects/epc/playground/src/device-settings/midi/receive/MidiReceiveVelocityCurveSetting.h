#pragma once
#include <device-settings/VelocityCurve.h>
#include <device-settings/EnumSetting.h>

class MidiReceiveVelocityCurveSetting : public EnumSetting<VelocityCurves>
{
 public:
  const std::vector<Glib::ustring>& enumToString() const override;
  const std::vector<Glib::ustring>& enumToDisplayString() const override;
  explicit MidiReceiveVelocityCurveSetting(UpdateDocumentContributor& s);
};

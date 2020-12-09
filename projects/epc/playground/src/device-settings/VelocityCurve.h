#pragma once

#include "EnumSetting.h"

enum class VelocityCurves
{
  VELOCITY_CURVE_VERY_SOFT = 0,
  VELOCITY_CURVE_SOFT = 1,
  VELOCITY_CURVE_NORMAL = 2,
  VELOCITY_CURVE_HARD = 3,
  VELOCITY_CURVE_VERY_HARD = 4,
};

class VelocityCurve : public EnumSetting<VelocityCurves>
{
 private:
  typedef EnumSetting<VelocityCurves> super;

 public:
  VelocityCurve(UpdateDocumentContributor &settings);
  virtual ~VelocityCurve();

  void syncExternals(SendReason reason) const override;

  const std::vector<Glib::ustring> &enumToDisplayString() const override;

 private:
  VelocityCurve(const VelocityCurve &other);
  VelocityCurve &operator=(const VelocityCurve &);

  const std::vector<Glib::ustring> &enumToString() const override;
};

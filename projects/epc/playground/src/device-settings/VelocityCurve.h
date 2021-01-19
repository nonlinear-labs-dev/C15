#pragma once

#include "EnumSetting.h"
#include <nltools/Types.h>

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

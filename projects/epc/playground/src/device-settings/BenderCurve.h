#pragma once

#include "EnumSetting.h"

enum class BenderCurves
{
  Soft = 0,
  Normal = 1,
  Hard = 2
};

class BenderCurve : public EnumSetting<BenderCurves>
{
 private:
  typedef EnumSetting<BenderCurves> super;

 public:
  BenderCurve(UpdateDocumentContributor &settings);
  virtual ~BenderCurve();

  void syncExternals(SendReason reason) const override;

  const std::vector<Glib::ustring> &enumToDisplayString() const override;

 private:
  const std::vector<Glib::ustring> &enumToString() const override;
};

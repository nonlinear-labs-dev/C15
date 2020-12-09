#pragma once

#include "EnumSetting.h"

enum class AftertouchCurves
{
  AFTERTOUCH_CURVE_SOFT = 0,
  AFTERTOUCH_CURVE_NORMAL = 1,
  AFTERTOUCH_CURVE_HARD = 2
};

class AftertouchCurve : public EnumSetting<AftertouchCurves>
{
 private:
  typedef EnumSetting<AftertouchCurves> super;

 public:
  AftertouchCurve(UpdateDocumentContributor &settings);
  virtual ~AftertouchCurve();

  void syncExternals(SendReason reason) const override;

  const std::vector<Glib::ustring> &enumToDisplayString() const override;

 private:
  AftertouchCurve(const AftertouchCurve &other);
  AftertouchCurve &operator=(const AftertouchCurve &);

  const std::vector<Glib::ustring> &enumToString() const override;
};

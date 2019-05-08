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
  AftertouchCurve(Settings &settings);
  virtual ~AftertouchCurve();

  void sendToLPC() const override;

  const vector<Glib::ustring> &enumToDisplayString() const override;

 private:
  AftertouchCurve(const AftertouchCurve &other);
  AftertouchCurve &operator=(const AftertouchCurve &);

  const vector<Glib::ustring> &enumToString() const override;
};

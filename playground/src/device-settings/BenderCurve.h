#pragma once

#include "EnumSetting.h"

enum class BenderCurves
{
  Soft = 0,
  Normal = 1,
  Hard = 2,
};

class BenderCurve : public EnumSetting<BenderCurves>
{
 private:
  typedef EnumSetting<BenderCurves> super;

 public:
  BenderCurve(Settings &settings);
  virtual ~BenderCurve();

  void sendToLPC() const override;

  const vector<Glib::ustring> &enumToDisplayString() const override;

 private:
  const vector<Glib::ustring> &enumToString() const override;
};

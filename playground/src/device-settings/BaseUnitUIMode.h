#pragma once

#include "EnumSetting.h"

enum class BaseUnitUIModes
{
  Play = 0,
  ParameterEdit = 1,
  Banks = 2,
  Presets = 3,
};

class BaseUnitUIMode : public EnumSetting<BaseUnitUIModes>
{
 private:
  typedef EnumSetting<BaseUnitUIModes> super;

 public:
  BaseUnitUIMode(Settings &settings);
  virtual ~BaseUnitUIMode();

  bool set(BaseUnitUIModes m) override;

 private:
  BaseUnitUIMode(const BaseUnitUIMode &other);
  BaseUnitUIMode &operator=(const BaseUnitUIMode &);

  const vector<Glib::ustring> &enumToString() const override;
  const vector<Glib::ustring> &enumToDisplayString() const override;
};

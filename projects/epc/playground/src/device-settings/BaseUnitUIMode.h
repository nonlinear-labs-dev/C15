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
  explicit BaseUnitUIMode(Settings &settings);
  ~BaseUnitUIMode() override;

  void syncExternals(SendReason reason) const override;

 private:
  BaseUnitUIMode(const BaseUnitUIMode &other);
  BaseUnitUIMode &operator=(const BaseUnitUIMode &);

  const std::vector<Glib::ustring> &enumToString() const override;
  const std::vector<Glib::ustring> &enumToDisplayString() const override;
};

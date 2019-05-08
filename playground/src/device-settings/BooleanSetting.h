#pragma once

#include "EnumSetting.h"

enum class BooleanSettings
{
  BOOLEAN_SETTING_TRUE = 0,
  BOOLEAN_SETTING_FALSE = 1
};

class BooleanSetting : public EnumSetting<BooleanSettings>
{
 private:
  typedef EnumSetting<BooleanSettings> super;

 public:
  BooleanSetting(Settings &settings, bool def);
  virtual ~BooleanSetting();

  void toggle();

  bool get() const;
  const vector<Glib::ustring> &enumToDisplayString() const override;

 private:
  BooleanSetting(const BooleanSetting &other) = delete;
  BooleanSetting &operator=(const BooleanSetting &) = delete;

  const vector<Glib::ustring> &enumToString() const override;
};

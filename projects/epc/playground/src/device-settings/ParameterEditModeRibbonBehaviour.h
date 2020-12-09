#pragma once

#include "EnumSetting.h"

enum class ParameterEditModeRibbonBehaviours
{
  PARAMETER_EDIT_MODE_RIBBON_BEHAVIOUR_RELATIVE = 0,
  PARAMETER_EDIT_MODE_RIBBON_BEHAVIOUR_ABSOLUTE = 1
};

class ParameterEditModeRibbonBehaviour : public EnumSetting<ParameterEditModeRibbonBehaviours>
{
 private:
  typedef EnumSetting<ParameterEditModeRibbonBehaviours> super;

 public:
  ParameterEditModeRibbonBehaviour(Settings &settings);
  virtual ~ParameterEditModeRibbonBehaviour();

  virtual bool set(ParameterEditModeRibbonBehaviours m) override;
  void sendRelatedSettingToPlaycontroller() const;

  void syncExternals(SendReason reason) const override;

 private:
  ParameterEditModeRibbonBehaviour(const ParameterEditModeRibbonBehaviour &other);

  ParameterEditModeRibbonBehaviour &operator=(const ParameterEditModeRibbonBehaviour &);

  const std::vector<Glib::ustring> &enumToString() const override;
  const std::vector<Glib::ustring> &enumToDisplayString() const override;
};

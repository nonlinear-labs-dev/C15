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
  void sendRelatedSettingToLPC() const;

 private:
  ParameterEditModeRibbonBehaviour(const ParameterEditModeRibbonBehaviour &other);
  ParameterEditModeRibbonBehaviour &operator=(const ParameterEditModeRibbonBehaviour &);

  void sendToLPC(SendReason reason) const override;

  const std::vector<Glib::ustring> &enumToString() const override;
  const std::vector<Glib::ustring> &enumToDisplayString() const override;
};

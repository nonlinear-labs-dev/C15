#pragma once
#include <parameters/ModulateableParameterWithUnusualModUnit.h>

class VoiceGroupMasterParameterWithUnusualModUnit : public ModulateableParameterWithUnusualModUnit
{
 public:
  VoiceGroupMasterParameterWithUnusualModUnit(ParameterGroup* group, const ParameterId& id, const ScaleConverter* scale,
                                              const ScaleConverter* modScale, tControlPositionValue def, int coarse,
                                              int fine, Glib::ustring longName, Glib::ustring shortName);
  Glib::ustring getLongName() const override;

  Glib::ustring getGroupAndParameterName() const override;

  DFBLayout* createLayout(FocusAndMode focusAndMode) const override;

  Glib::ustring getShortName() const override;

 private:
  std::string m_shortname;
  std::string m_longname;
};

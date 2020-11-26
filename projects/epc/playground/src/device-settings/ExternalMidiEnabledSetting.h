#pragma once
#include "BooleanSetting.h"

class ExternalMidiEnabledSetting : public BooleanSetting
{
 public:
  explicit ExternalMidiEnabledSetting(UpdateDocumentContributor& s);
  const std::vector<Glib::ustring>& enumToString() const override;
  void setSetting(Initiator initiator, const Glib::ustring& text) override;

 private:
  void onSettingChanged();
  const std::vector<Glib::ustring> m_enumToStringMap;
};

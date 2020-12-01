#pragma once
#include "BooleanSetting.h"

class ExternalMidiEnabledSetting : public BooleanSetting
{
 public:
  explicit ExternalMidiEnabledSetting(UpdateDocumentContributor& s);
  const std::vector<Glib::ustring>& enumToString() const override;
  bool set(tEnum m) override;

 private:
  void onSettingChanged();
  const std::vector<Glib::ustring> m_enumToStringMap;
};

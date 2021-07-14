#pragma once
#include <device-settings/EnumSetting.h>
#include <nltools/Types.h>

class BenderCCMapping : public EnumSetting<BenderCC>
{
 public:
  BenderCCMapping(UpdateDocumentContributor& u);
  const std::vector<Glib::ustring>& enumToString() const override;
  const std::vector<Glib::ustring>& enumToDisplayString() const override;
};

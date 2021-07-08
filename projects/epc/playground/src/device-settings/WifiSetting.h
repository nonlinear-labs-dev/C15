#pragma once
#include <nltools/enums/EnumTools.h>
#include "EpcWifi.h"
#include "NLEnumSetting.h"

ENUM(WifiSettings, int, Disabled, Enabled)

class WifiSetting : public NLEnumSetting<WifiSettings>, public sigc::trackable
{
 public:
  typedef NLEnumSetting<WifiSettings> super;
  explicit WifiSetting(UpdateDocumentContributor& settings, std::shared_ptr<EpcWifi>  localWifi);

  bool set(tEnum m) override;
  bool persistent() const override;

 private:
  void enableDisableBBBWifi(tEnum state);

  sigc::connection m_pollConnection;
  const std::shared_ptr<EpcWifi> m_localWifi;
};

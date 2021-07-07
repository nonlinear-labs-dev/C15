#pragma once
#include <nltools/enums/EnumTools.h>
#include "EpcWifi.h"
#include "NLEnumSetting.h"

ENUM(WifiSettings, int, Disabled, Enabled, Querying)

class WifiSetting;

class WiFiPollImpl {
 public:
  explicit WiFiPollImpl(WifiSetting* s);
  virtual bool poll() = 0;
 protected:
  WifiSetting* m_setting;
};

class EPC2WiFiPollImpl : public WiFiPollImpl {
 public:
  using WiFiPollImpl::WiFiPollImpl;
  bool poll() override;
};

class BBBWiFiPollImpl : public WiFiPollImpl {
 public:
  using WiFiPollImpl::WiFiPollImpl;
  bool poll() override;
};

class WifiSetting : public NLEnumSetting<WifiSettings>, public sigc::trackable
{
 public:
  typedef NLEnumSetting<WifiSettings> super;
  explicit WifiSetting(UpdateDocumentContributor& settings, const std::shared_ptr<EpcWifi>& localWifi);

  bool set(tEnum m) override;
  bool persistent() const override;

 private:
  void enableDisableBBBWifi(tEnum state);
  bool pollAccessPointRunning();

  sigc::connection m_pollConnection;
  const std::shared_ptr<EpcWifi> m_localWifi;

  std::unique_ptr<WiFiPollImpl> m_pollImpl;
};

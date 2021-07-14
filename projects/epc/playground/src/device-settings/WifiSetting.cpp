#include "WifiSetting.h"
#include <nltools/system/SpawnCommandLine.h>
#include <nltools/Types.h>
#include <nltools/logging/Log.h>
#include <playground.h>
#include <utility>
#include <nltools/messaging/Messaging.h>
#include <nltools/messaging/Message.h>

WifiSetting::WifiSetting(UpdateDocumentContributor& settings, std::shared_ptr<EpcWifi> localWifi)
    : super(settings, WifiSettings::Enabled)
    , m_localWifi(std::move(localWifi))
{
  nltools::msg::onConnectionEstablished(nltools::msg::EndPoint::BeagleBone, [this](){
     m_connectionToBBBEstablished = true;
     setupBBBWifiIfBBBConnectedAndSettingLoaded();
  });
}

bool WifiSetting::set(tEnum m)
{
  auto didChange = super::set(m);

  if(m == WifiSettings::Enabled)
    m_localWifi->setNewWifiState(true);
  else if(m == WifiSettings::Disabled)
    m_localWifi->setNewWifiState(false);

  if(!isLoading()) {
    setupBBBWifiIfBBBConnectedAndSettingLoaded();
  }

  return didChange;
}

bool WifiSetting::persistent() const
{
  return true;
}

bool WifiSetting::isLoading() const
{
  return m_isLoading;
}

void WifiSetting::load(const Glib::ustring& text, Initiator initiator)
{
  m_isLoading = true;
  NLEnumSetting::load(text, initiator);

  if(initiator == Initiator::EXPLICIT_LOAD) {
    m_didSettingLoad = true;
    setupBBBWifiIfBBBConnectedAndSettingLoaded();
  }

  m_isLoading = false;
}

void WifiSetting::enableDisableBBBWifi(WifiSettings m)
{
    using namespace nltools::msg;
    WiFi::EnableWiFiMessage message { m == WifiSettings::Enabled };
    send(EndPoint::BeagleBone, message);
}

void WifiSetting::setupBBBWifiIfBBBConnectedAndSettingLoaded()
{
  nltools::Log::error(__LINE__, "bbbb there:", m_connectionToBBBEstablished, "loaded", m_didSettingLoad);

  if(m_connectionToBBBEstablished && m_didSettingLoad)
    enableDisableBBBWifi(get());
}

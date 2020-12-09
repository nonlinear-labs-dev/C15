#include <nltools/messaging/Message.h>
#include "ExternalMidiEnabledSetting.h"

ExternalMidiEnabledSetting::ExternalMidiEnabledSetting(UpdateDocumentContributor& s)
    : BooleanSetting(s, false)
{
  nltools::msg::onConnectionEstablished(nltools::msg::EndPoint::ExternalMidiOverIPBridge,
                                        [this] { syncExternals(SendReason::SettingsLoaded); });
}

ExternalMidiEnabledSetting::~ExternalMidiEnabledSetting() = default;

void ExternalMidiEnabledSetting::syncExternals(SendReason reason) const
{
  using namespace nltools;
  msg::Setting::MidiBridgeSettings m { get() };
  send(msg::EndPoint::ExternalMidiOverIPBridge, m);
  nltools::Log::warning("Set MidiEnabled setting to:", m.enable ? "true" : "false");
}
#include <nltools/messaging/Message.h>
#include "ExternalMidiEnabledSetting.h"

const std::vector<Glib::ustring>& ExternalMidiEnabledSetting::enumToString() const
{
  return m_enumToStringMap;
}

ExternalMidiEnabledSetting::ExternalMidiEnabledSetting(UpdateDocumentContributor& s)
    : BooleanSetting(s, false)
    , m_enumToStringMap { "true", "false" }
{
}

bool ExternalMidiEnabledSetting::set(BooleanSettings m)
{
  auto ret = EnumSetting::set(m);
  onSettingChanged();
  return ret;
}

void ExternalMidiEnabledSetting::load(const Glib::ustring& text, Initiator initiator)
{
  EnumSetting::load(text, initiator);
  onSettingChanged();
}

void ExternalMidiEnabledSetting::onSettingChanged()
{
  using namespace nltools;
  msg::Setting::MidiEnabled m;
  m.enable = get();
  send(msg::EndPoint::ExternalMidiOverIPBridgeSettings, m);
  nltools::Log::warning("Set MidiEnabled setting to:", m.enable ? "true" : "false");
}
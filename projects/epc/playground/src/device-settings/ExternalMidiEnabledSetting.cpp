#include <nltools/messaging/Message.h>
#include "ExternalMidiEnabledSetting.h"

const std::vector<Glib::ustring>& ExternalMidiEnabledSetting::enumToString() const
{
  return m_enumToStringMap;
}

ExternalMidiEnabledSetting::ExternalMidiEnabledSetting(UpdateDocumentContributor& s)
    : BooleanSetting(s, false)
    , m_enumToStringMap { "On", "Off" }
{
}

bool ExternalMidiEnabledSetting::set(BooleanSettings m)
{
  auto ret = EnumSetting::set(m);
  onSettingChanged();
  return ret;
}

void ExternalMidiEnabledSetting::onSettingChanged()
{
  using namespace nltools;
  msg::Setting::MidiEnabled m;
  m.enable = get();
  send(msg::EndPoint::ExternalMidiOverIPBridgeSettings, m);
}
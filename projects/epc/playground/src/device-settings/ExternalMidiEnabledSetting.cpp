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

void ExternalMidiEnabledSetting::onSettingChanged()
{
  using namespace nltools;
  msg::Setting::MidiEnabled m;
  m.enable = get();
  send(msg::EndPoint::ExternalMidiOverIPBridge, m);
}

void ExternalMidiEnabledSetting::setSetting(Initiator initiator, const Glib::ustring& text)
{
  Setting::setSetting(initiator, text);
  onSettingChanged();
}

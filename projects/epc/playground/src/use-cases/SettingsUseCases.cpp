#include "SettingsUseCases.h"
#include <device-settings/Settings.h>
#include <device-settings/midi/mappings/PedalCCMapping.h>
#include <device-settings/midi/mappings/RibbonCCMapping.h>
#include <device-settings/midi/mappings/AftertouchCCMapping.h>
#include <device-settings/midi/mappings/BenderCCMapping.h>
#include <device-settings/midi/mappings/Enable14BitSupport.h>
#include <device-settings/midi/mappings/EnableHighVelocityCC.h>
#include <device-settings/midi/HardwareControlEnables.h>
#include <nltools/messaging/Message.h>

SettingsUseCases::SettingsUseCases(Settings *s)
    : m_settings { s }
{
}

void SettingsUseCases::setMappingsToHighRes()
{
  auto fourteenBitSupport = m_settings->getSetting<Enable14BitSupport>();
  auto velCC = m_settings->getSetting<EnableHighVelocityCC>();
  auto p1 = m_settings->getSetting<PedalCCMapping<1>>();
  auto p2 = m_settings->getSetting<PedalCCMapping<2>>();
  auto p3 = m_settings->getSetting<PedalCCMapping<3>>();
  auto p4 = m_settings->getSetting<PedalCCMapping<4>>();
  auto r1 = m_settings->getSetting<RibbonCCMapping<1>>();
  auto r2 = m_settings->getSetting<RibbonCCMapping<2>>();
  auto at = m_settings->getSetting<AftertouchCCMapping>();
  auto be = m_settings->getSetting<BenderCCMapping>();

  fourteenBitSupport->set(BooleanSettings::BOOLEAN_SETTING_TRUE);
  velCC->set(BooleanSettings::BOOLEAN_SETTING_TRUE);
  p1->set(PedalCC::CC20);
  p2->set(PedalCC::CC21);
  p3->set(PedalCC::CC22);
  p4->set(PedalCC::CC23);
  r1->set(RibbonCC::CC24);
  r2->set(RibbonCC::CC25);
  at->set(AftertouchCC::CC26);
  be->set(BenderCC::Pitchbend);
}

void SettingsUseCases::setMappingsToClassicMidi()
{
  auto fourteenBitSupport = m_settings->getSetting<Enable14BitSupport>();
  auto velCC = m_settings->getSetting<EnableHighVelocityCC>();
  auto p1 = m_settings->getSetting<PedalCCMapping<1>>();
  auto p2 = m_settings->getSetting<PedalCCMapping<2>>();
  auto p3 = m_settings->getSetting<PedalCCMapping<3>>();
  auto p4 = m_settings->getSetting<PedalCCMapping<4>>();
  auto r1 = m_settings->getSetting<RibbonCCMapping<1>>();
  auto r2 = m_settings->getSetting<RibbonCCMapping<2>>();
  auto at = m_settings->getSetting<AftertouchCCMapping>();
  auto be = m_settings->getSetting<BenderCCMapping>();

  fourteenBitSupport->set(BooleanSettings::BOOLEAN_SETTING_FALSE);
  velCC->set(BooleanSettings::BOOLEAN_SETTING_FALSE);
  p1->set(PedalCC::CC20);
  p2->set(PedalCC::CC21);
  p3->set(PedalCC::CC22);
  p4->set(PedalCC::CC23);
  r1->set(RibbonCC::CC24);
  r2->set(RibbonCC::CC25);
  at->set(AftertouchCC::ChannelPressure);
  be->set(BenderCC::Pitchbend);
}

void SettingsUseCases::updateHWSourceEnable(int hw, int aspect, bool value)
{
  if(auto s = m_settings->getSetting<HardwareControlEnables>())
  {
    if(hw < (int) HardwareControlEnables::tHWIdx::LENGTH && aspect < (int) HardwareControlEnables::tSettingIdx::LENGTH)
    {
      s->setState(static_cast<HardwareControlEnables::tHWIdx>(hw),
                  static_cast<HardwareControlEnables::tSettingIdx>(aspect), value);
    }
  }
}

void SettingsUseCases::panicAudioEngine()
{
  using namespace nltools::msg;
  PanicAudioEngine msg {};
  send<PanicAudioEngine>(EndPoint::AudioEngine, msg);
}

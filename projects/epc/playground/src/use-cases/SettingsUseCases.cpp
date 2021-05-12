#include "SettingsUseCases.h"
#include <device-settings/Settings.h>
#include <device-settings/midi/mappings/PedalCCMapping.h>
#include <device-settings/midi/mappings/RibbonCCMapping.h>
#include <device-settings/midi/mappings/AftertouchCCMapping.h>
#include <device-settings/midi/mappings/BenderCCMapping.h>
#include <device-settings/midi/mappings/Enable14BitSupport.h>
#include <device-settings/midi/mappings/EnableHighVelocityCC.h>

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
  p1->set(PedalCC::CC01);
  p2->set(PedalCC::CC02);
  p3->set(PedalCC::CC03);
  p4->set(PedalCC::CC04);
  r1->set(RibbonCC::CC05);
  r2->set(RibbonCC::CC06);
  at->set(AftertouchCC::ChannelPressure);
  be->set(BenderCC::Pitchbend);
}

#include "BaseUnit.h"
#include "BaseUnitPlayMode.h"
#include "BaseUnitEditMode.h"
#include "BaseUnitPresetsMode.h"
#include "BaseUnitBanksMode.h"
#include "Application.h"
#include "device-settings/Settings.h"
#include "device-settings/BaseUnitUIMode.h"
#include <nltools/messaging/Message.h>
#include <proxies/hwui/HWUI.h>

BaseUnit::BaseUnit()
{
  Application::get().getSettings()->getSetting<BaseUnitUIMode>()->onChange(mem_fun(this, &BaseUnit::respectUsageMode));
  nltools::msg::onConnectionEstablished(nltools::msg::EndPoint::RibbonLed,
                                        sigc::mem_fun(this, &BaseUnit::onBBBBConnected));
}

BaseUnit::~BaseUnit() = default;

void BaseUnit::init()
{
  m_playPanel.getSOLED().init();
}

void BaseUnit::onBBBBConnected()
{
  m_upperRibbon.syncBBBB();
  m_lowerRibbon.syncBBBB();
}

void BaseUnit::respectUsageMode(const Setting *s)
{
  auto m = dynamic_cast<const BaseUnitUIMode *>(s);

  switch(m->get())
  {
    case BaseUnitUIModes::Play:
      setUsageMode(new BaseUnitPlayMode());
      break;

    case BaseUnitUIModes::ParameterEdit:
      setUsageMode(new BaseUnitEditMode());
      break;

    case BaseUnitUIModes::Banks:
      setUsageMode(new BaseUnitBanksMode());
      break;

    case BaseUnitUIModes::Presets:
      setUsageMode(new BaseUnitPresetsMode());
      break;
  }
}

void BaseUnit::indicateBlockingMainThread(bool onOff)
{
  m_lowerRibbon.indicateBlockingMainThread(onOff);
}

void BaseUnit::onTimeout()
{
  m_playPanel.onTimeout();
}

PlayPanel &BaseUnit::getPlayPanel()
{
  return m_playPanel;
}
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/playcontroller/PlaycontrollerProxy.h>
#include <proxies/hwui/panel-unit/PanelUnit.h>
#include <proxies/hwui/panel-unit/EditPanel.h>
#include <proxies/hwui/panel-unit/boled/BOLED.h>
#include <proxies/hwui/panel-unit/boled/BOLEDScreenSaver.h>
#include "ScreenSaverTimeoutSetting.h"

ScreenSaverTimeoutSetting::ScreenSaverTimeoutSetting(UpdateDocumentContributor& parent)
    : Setting(parent)
{
}

void ScreenSaverTimeoutSetting::load(const Glib::ustring& text, Initiator initiator)
{
  try
  {
    auto loadedValue = std::stoi(text);

    selectedIndex = 0;

    for(auto i = 0; i < s_logTimeOuts.size(); i++)
    {
      auto v = s_logTimeOuts[i];
      if(loadedValue == v)
      {
        selectedIndex = i;
        break;
      }
    }

    m_timeout = std::chrono::minutes(s_logTimeOuts[selectedIndex]);
  }
  catch(...)
  {
    m_timeout = std::chrono::minutes(0);
    selectedIndex = 0;
  }
}

Glib::ustring ScreenSaverTimeoutSetting::save() const
{
  return std::to_string(m_timeout.count());
}

bool ScreenSaverTimeoutSetting::persistent() const
{
  return true;
}

Glib::ustring ScreenSaverTimeoutSetting::getDisplayString() const
{
  if(m_timeout.count() == 0)
    return "Disabled";
  if(m_timeout.count() == 1)
    return "1 Minute";
  if(m_timeout.count() == 60)
    return "1 Hour";
  if(m_timeout.count() > 60)
    return std::to_string(m_timeout.count() / 60) + " Hours";

  return std::to_string(m_timeout.count()) + " Minutes";
}

sigc::connection ScreenSaverTimeoutSetting::onScreenSaverStateChanged(sigc::slot<void, bool> s)
{
  return m_screenSaverSignal.connect(s);
}

void ScreenSaverTimeoutSetting::init()
{
  if(m_timeout.count() > 0)
  {
    m_expiration = std::make_unique<Expiration>([&] { sendState(true); }, m_timeout);
  }

  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      [this](Parameter* n, Parameter* old, SignalOrigin type) { endAndReschedule(); }, std::nullopt);

  Application::get().getPresetManager()->getEditBuffer()->onChange([this]() { endAndReschedule(); }, false);

  Application::get().getPlaycontrollerProxy()->onLastKeyChanged([this](int key) { endAndReschedule(); });

  Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().onLayoutInstalled([this](Layout* l) {
    if(dynamic_cast<BOLEDScreenSaver*>(l) == nullptr)
    {
      endAndReschedule();
    }
  });
}

void ScreenSaverTimeoutSetting::sendState(bool state)
{
  m_screenSaverSignal.send(state);
}

void ScreenSaverTimeoutSetting::endAndReschedule()
{
  sendState(false);

  if(m_expiration)
  {
    m_expiration->refresh(m_timeout);
  }
}

void ScreenSaverTimeoutSetting::incDec(int inc)
{

  selectedIndex = std::min<int>(std::max<int>(selectedIndex + inc, 0), s_logTimeOuts.size() - 1);
  m_timeout = std::chrono::minutes(s_logTimeOuts[selectedIndex]);

  if(m_timeout.count() == 0)
  {
    m_expiration.reset(nullptr);
    sendState(false);
  }
  else
  {
    if(m_expiration)
    {
      m_expiration->refresh(m_timeout);
    }
    else
    {
      m_expiration = std::make_unique<Expiration>([&] { sendState(true); }, m_timeout);
    }
  }

  notify();
}

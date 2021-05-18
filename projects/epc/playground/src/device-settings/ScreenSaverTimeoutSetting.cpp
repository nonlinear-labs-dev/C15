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
#include <device-settings/Settings.h>

ScreenSaverTimeoutSetting::ScreenSaverTimeoutSetting(UpdateDocumentContributor& parent)
    : Setting(parent)
{
  nltools_assertOnDevPC(s_logTimeOuts.size() == s_displayStrings.size());
}

void ScreenSaverTimeoutSetting::load(const Glib::ustring& text, Initiator initiator)
{
  try
  {
    auto loadedValue = std::stoi(text);

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

int ScreenSaverTimeoutSetting::getSelectedIndex() const
{
  return selectedIndex;
}

Glib::ustring ScreenSaverTimeoutSetting::getDisplayString() const
{
  return s_displayStrings.at(getSelectedIndex());
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
      [this](auto* n, auto* old) { endAndReschedule(); }, std::nullopt);

  Application::get().getPresetManager()->getEditBuffer()->onChange([this]() { endAndReschedule(); }, false);

  Application::get().getPlaycontrollerProxy()->onLastKeyChanged([this](int key) { endAndReschedule(); });

  Application::get().getSettings()->onSettingsChanged(
      sigc::mem_fun(this, &ScreenSaverTimeoutSetting::endAndReschedule));

  Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().onLayoutInstalled(
      sigc::mem_fun(this, &ScreenSaverTimeoutSetting::onLayoutInstalled));
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

void ScreenSaverTimeoutSetting::onLayoutInstalled(Layout* l)
{
  if(dynamic_cast<BOLEDScreenSaver*>(l) == nullptr)
  {
    endAndReschedule();
  }
}

void ScreenSaverTimeoutSetting::incDec(int inc, ButtonModifiers m)
{
  incDec(inc);
}

const std::vector<Glib::ustring>& ScreenSaverTimeoutSetting::getDisplayStrings() const
{
  return s_displayStrings;
}

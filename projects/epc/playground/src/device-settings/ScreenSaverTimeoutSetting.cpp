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
#include <nltools/messaging/Message.h>

static constexpr auto c_disabled = std::chrono::minutes::zero();
static constexpr std::array<int, 6> s_logTimeOuts = { 0, 1, 5, 20, 60, 180 };
static const std::vector<Glib::ustring> s_displayStrings
    = { "Disabled", "1 Minute", "5 Minutes", "20 Minutes", "1 Hour", "3 Hours" };

ScreenSaverTimeoutSetting::ScreenSaverTimeoutSetting(UpdateDocumentContributor& parent)
    : Setting(parent)
    , m_expiration([this] { sendState(true); })
{
  nltools_assertOnDevPC(s_logTimeOuts.size() == s_displayStrings.size());
}

void ScreenSaverTimeoutSetting::load(const Glib::ustring& text, Initiator initiator)
{
  try
  {
    auto loadedValue = std::stoi(text);

    if(auto pos = std::find(s_logTimeOuts.begin(), s_logTimeOuts.end(), loadedValue); pos != s_logTimeOuts.end())
      m_selectedIndex = std::distance(s_logTimeOuts.begin(), pos);

    m_timeout = std::chrono::minutes(s_logTimeOuts[m_selectedIndex]);
  }
  catch(...)
  {
    m_timeout = c_disabled;
    m_selectedIndex = 0;
  }
}

Glib::ustring ScreenSaverTimeoutSetting::save() const
{
  return std::to_string(m_timeout.count());
}

int ScreenSaverTimeoutSetting::getSelectedIndex() const
{
  return m_selectedIndex;
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
  if(m_timeout != c_disabled)
    m_expiration.refresh(m_timeout);

  auto& app = Application::get();
  auto editBuffer = app.getPresetManager()->getEditBuffer();
  auto reschedule = sigc::mem_fun(this, &ScreenSaverTimeoutSetting::endAndReschedule);

  editBuffer->onSelectionChanged(sigc::hide(sigc::hide(reschedule)), std::nullopt);
  editBuffer->onChange(reschedule, false);
  app.getPlaycontrollerProxy()->onLastKeyChanged(sigc::hide(reschedule));
  app.getSettings()->onSettingsChanged(reschedule);
  app.getHWUI()->getPanelUnit().getEditPanel().getBoled().onLayoutInstalled(
      sigc::mem_fun(this, &ScreenSaverTimeoutSetting::onLayoutInstalled));

  nltools::msg::receive<nltools::msg::Keyboard::ActionHappened>(nltools::msg::EndPoint::Playground, sigc::mem_fun(this, &ScreenSaverTimeoutSetting::onKeyBedMessageReceived));
}

void ScreenSaverTimeoutSetting::sendState(bool state)
{
  m_screenSaverSignal.send(state);
}

void ScreenSaverTimeoutSetting::endAndReschedule()
{
  sendState(false);

  if(m_timeout != c_disabled)
    m_expiration.refresh(m_timeout);
}

void ScreenSaverTimeoutSetting::incDec(int inc)
{
  m_selectedIndex = std::clamp<int>(m_selectedIndex + inc, 0, s_logTimeOuts.size() - 1);
  m_timeout = std::chrono::minutes(s_logTimeOuts[m_selectedIndex]);

  if(m_timeout != c_disabled)
    m_expiration.refresh(m_timeout);
  else
    m_expiration.cancel();

  notify();
}

void ScreenSaverTimeoutSetting::onLayoutInstalled(Layout* l)
{
  if(!dynamic_cast<BOLEDScreenSaver*>(l))
    endAndReschedule();
}

void ScreenSaverTimeoutSetting::incDec(int inc, ButtonModifiers m)
{
  incDec(inc);
}

const std::vector<Glib::ustring>& ScreenSaverTimeoutSetting::getDisplayStrings() const
{
  return s_displayStrings;
}

void ScreenSaverTimeoutSetting::onKeyBedMessageReceived(const nltools::msg::Keyboard::ActionHappened& m)
{
  endAndReschedule();
}

#include <proxies/hwui/controls/Label.h>
#include <proxies/hwui/OLEDProxy.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include "BOLEDScreenSaver.h"
#include <device-settings/Settings.h>
#include <device-settings/Setting.h>
#include <device-settings/ScreenSaverTimeoutSetting.h>
#include <glibmm/main.h>
#include <chrono>
#include <device-settings/Settings.h>
#include <device-settings/ScreenSaverTimeoutSetting.h>

BOLEDScreenSaver::BOLEDScreenSaver(OLEDProxy& oled)
    : Layout(oled)
{

  m_animation = Application::get().getMainContext()->signal_timeout().connect(
      sigc::mem_fun(this, &BOLEDScreenSaver::animate), 50);

  m_editBufferConnection = Application::get().getPresetManager()->getEditBuffer()->onChange(
      sigc::mem_fun(this, &BOLEDScreenSaver::destroy), false);
}

void BOLEDScreenSaver::init()
{
  m_label = addControl(new Label(StringAndSuffix { "C15", 0 }, { 128, 40, 18, 9 }));
  Layout::init();
}

BOLEDScreenSaver::~BOLEDScreenSaver()
{
  m_animation.disconnect();
}

bool BOLEDScreenSaver::animate()
{
  if(m_label)
  {
    auto old = m_label->getPosition();

    if(old.getRight() >= 256)
    {
      m_vel.first = -1;
    }

    if(old.getLeft() <= 0)
    {
      m_vel.first = 1;
    }

    if(old.getBottom() >= 64)
    {
      m_vel.second = -1;
    }

    if(old.getTop() <= 0)
    {
      m_vel.second = 1;
    }

    old.setLeft(old.getLeft() + m_vel.first);
    old.setTop(old.getTop() + m_vel.second);

    m_label->setPosition(old);

    setDirty();
  }
  return true;
}

void BOLEDScreenSaver::destroy()
{
  getOLEDProxy().resetOverlay();
}

bool BOLEDScreenSaver::onRotary(int inc, ButtonModifiers modifiers)
{
  Application::get().getSettings()->getSetting<ScreenSaverTimeoutSetting>()->endAndReschedule();
  return true;
}

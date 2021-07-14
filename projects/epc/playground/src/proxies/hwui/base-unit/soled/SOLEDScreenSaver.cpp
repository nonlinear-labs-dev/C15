#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include "SOLEDScreenSaver.h"
#include "proxies/hwui/OLEDProxy.h"
#include "proxies/hwui/FrameBuffer.h"
#include <glibmm/main.h>

SOLEDScreenSaver::SOLEDScreenSaver(OLEDProxy& oled)
    : Layout(oled)
{

  m_animationConnection = Application::get().getMainContext()->signal_timeout().connect(
      sigc::mem_fun(this, &SOLEDScreenSaver::animate), 100);
  m_editbufferConnection = Application::get().getPresetManager()->getEditBuffer()->onChange(
      sigc::mem_fun(this, &SOLEDScreenSaver::destroy), false);
}

bool SOLEDScreenSaver::animate()
{
  auto old = m_scrollingLabel->getPosition();

  auto& velX = velocity.first;
  auto& velY = velocity.second;

  old.setLeft(old.getLeft() + velX);
  old.setTop(old.getTop() + velY);

  if(old.getLeft() <= 0)
    velX = 1;
  else if(old.getRight() >= 127)
    velX = -1;

  if(old.getTop() <= 0)
    velY = 1;
  else if(old.getBottom() >= 32)
    velY = -1;

  m_scrollingLabel->setPosition(old);
  return true;
}

bool SOLEDScreenSaver::redraw(FrameBuffer& fb)
{
  fb.setColor(FrameBufferColors::C43);
  fb.fillRect(0, 0, 128, 32);
  return ControlOwner::redraw(fb);
}

void SOLEDScreenSaver::init()
{
  m_scrollingLabel = addControl(new Label(StringAndSuffix { "C15", 0 }, { 1, 0, 18, 9 }));
}

void SOLEDScreenSaver::destroy()
{
  getOLEDProxy().resetOverlay();
}

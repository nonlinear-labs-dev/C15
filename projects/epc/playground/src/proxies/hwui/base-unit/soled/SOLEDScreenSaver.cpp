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
      sigc::mem_fun(this, &SOLEDScreenSaver::animate), 50);
  m_editbufferConnection = Application::get().getPresetManager()->getEditBuffer()->onChange(
      sigc::mem_fun(this, &SOLEDScreenSaver::destroy), false);
}

bool SOLEDScreenSaver::animate()
{
  auto old = m_scrollingLabel->getPosition();

  old.setLeft(old.getLeft() + velocity);

  if(old.getLeft() <= 1)
    velocity = 1;
  else if(old.getRight() >= 127)
    velocity = -1;

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
  m_scrollingLabel = addControl(new Label({ "Nonlinear Labs - C15", 0 }, { 1, 0, 93, 32 }));
}

void SOLEDScreenSaver::destroy()
{
  getOLEDProxy().resetOverlay();
}

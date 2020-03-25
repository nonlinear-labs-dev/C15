#include <glib.h>
#include "FourStateLED.h"
#include "device-settings/DebugLevel.h"
#include <Application.h>
#include <nltools/messaging/Message.h>

FourStateLED::FourStateLED()
{
}

FourStateLED::~FourStateLED()
{
  setState(State::Bright);
}

void FourStateLED::setState(State state)
{
  if(m_state != state)
  {
    m_state = state;
    syncHWUI();
  }
}

void FourStateLED::syncHWUI()
{
  using namespace nltools::msg;
  SetRibbonLEDMessage msg;
  msg.id = static_cast<uint8_t>(getID());
  msg.brightness = static_cast<uint8_t>(m_state & 0x03);
  send(EndPoint::RibbonLed, msg);
}

FourStateLED::State FourStateLED::getState() const
{
  return m_state;
}

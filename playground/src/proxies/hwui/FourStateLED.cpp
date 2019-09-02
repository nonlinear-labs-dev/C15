#include <glib.h>
#include "FourStateLED.h"
#include "device-settings/DebugLevel.h"
#include <Application.h>
#include <nltools/messaging/Message.h>

FourStateLED::FourStateLED()
{
  m_state = -1;
}

FourStateLED::~FourStateLED() = default;

void FourStateLED::setState(char state)
{
  if(m_state != state)
  {
    m_state = state;
    syncBBBB();
  }
}

void FourStateLED::syncBBBB()
{
  using namespace nltools::msg;
  SetRibbonLEDMessage msg;
  msg.id = static_cast<uint8_t>(getID());
  msg.brightness = static_cast<uint8_t>(m_state & 0x03);
  send(EndPoint::RibbonLed, msg);
}

char FourStateLED::getState() const
{
  return m_state;
}

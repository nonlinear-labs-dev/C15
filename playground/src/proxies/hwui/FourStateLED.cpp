#include <glib.h>
#include "FourStateLED.h"
#include "device-settings/DebugLevel.h"
#include <io/network/WebSocketSession.h>
#include <Application.h>

FourStateLED::FourStateLED()
{
  m_state = -1;
}

FourStateLED::~FourStateLED()
{
}

void FourStateLED::setState(char state, bool flush)
{
  if(m_state != state)
  {
    m_state = state;
    syncBBBB();
  }
}

void FourStateLED::syncBBBB()
{
  auto data = new uint8_t[2];
  data[0] = getID();
  data[1] = (m_state & 0x03);

  auto msg = Glib::Bytes::create(data, 2);
  Application::get().getWebSocketSession()->sendMessage(WebSocketSession::Domain::RibbonLed, msg);
}

char FourStateLED::getState() const
{
  return m_state;
}

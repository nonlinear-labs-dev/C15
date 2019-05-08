#include <glib.h>
#include <fstream>
#include "TwoStateLED.h"
#include "device-settings/DebugLevel.h"
#include "Application.h"
#include "proxies/hwui/HWUI.h"
#include "proxies/hwui/debug-oled/DebugLeds.h"
#include "proxies/hwui/debug-oled/DebugOLED.h"

TwoStateLED::TwoStateLED(int id)
    : m_state(OFF)
{
  setID(id);
}

TwoStateLED::~TwoStateLED()
{
}

void TwoStateLED::init()
{
  switchLED(false);
}

void TwoStateLED::setState(LedState state)
{
  if(m_state != state)
  {
    m_state = state;
    DebugLevel::info("TwoStateLED::setState", getID(), m_state);

    switch(m_state)
    {
      case BLINK:
        m_blinkTimer
            = Application::get().getHWUI()->connectToBlinkTimer(sigc::mem_fun(this, &TwoStateLED::onBlinkUpdate));
        break;

      case ON:
        m_blinkTimer.disconnect();
        switchLED(true);
        break;

      case OFF:
        m_blinkTimer.disconnect();
        switchLED(false);
        break;
    }
  }
}

TwoStateLED::LedState TwoStateLED::getState()
{
  return m_state;
}

void TwoStateLED::onBlinkUpdate(int blinkCount)
{
  if(m_state == BLINK)
    switchLED(blinkCount % 2);
}

void TwoStateLED::switchLED(bool onOrOff)
{
  uint8_t *val = new uint8_t[1];
  *val = ((onOrOff ? 1 : 0) << 7) | (getID() & 0x7F);
  auto msg = Glib::Bytes::create(val, 1);
  Application::get().getWebSocketSession()->sendMessage(WebSocketSession::Domain::PanelLed, msg);
}

void TwoStateLED::syncBBBB()
{
  if(m_state == ON)
    switchLED(true);
  else if(m_state == OFF)
    switchLED(false);
}

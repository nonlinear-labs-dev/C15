#include <Application.h>
#include <device-settings/DebugLevel.h>
#include <device-settings/EncoderAcceleration.h>
#include <device-settings/Settings.h>
#include <proxies/hwui/panel-unit/RotaryEncoder.h>
#include <testing/TestDriver.h>
#include <proxies/lpc/LPCProxy.h>

static TestDriver<RotaryEncoder> tester;

RotaryEncoder::RotaryEncoder()
    : m_throttler(chrono::milliseconds(25))
{
  Application::get().getWebSocketSession()->onMessageReceived(WebSocketSession::Domain::Rotary,
                                                              sigc::mem_fun(this, &RotaryEncoder::onMessage));
}

RotaryEncoder::~RotaryEncoder()
{
  m_stress.disconnect();
}

void RotaryEncoder::onMessage(WebSocketSession::tMessage msg)
{
  gsize numBytes = 0;
  const char *buffer = (const char *) msg->get_data(numBytes);

  if(numBytes > 0)
    applyIncrement(buffer[0]);
}

void RotaryEncoder::applyIncrement(tIncrement currentInc)
{
  m_signalRotaryChanged.send(currentInc);

  if((currentInc < 0) != (m_accumulatedIncs < 0))
    m_accumulatedIncs = 0;

  m_accumulatedIncs += currentInc;
  m_throttler.doTask([this]() {
    if(abs(m_accumulatedIncs) > 1)
    {
      m_accumulatedIncs = std::min(m_accumulatedIncs, 10);
      m_accumulatedIncs = std::max(m_accumulatedIncs, -10);
      double factor = Application::get().getSettings()->getSetting<EncoderAcceleration>()->get();
      int sign = m_accumulatedIncs < 0 ? -1 : 1;
      m_signalRotaryChanged.send(factor * sign * m_accumulatedIncs * m_accumulatedIncs);
    }
    m_accumulatedIncs = 0;
  });
}

void RotaryEncoder::fake(tIncrement i)
{
  m_signalRotaryChanged.send(i);
}

sigc::connection RotaryEncoder::onRotaryChanged(function<void(tIncrement)> slot)
{
  return m_signalRotaryChanged.connect(slot);
}

void RotaryEncoder::registerTests()
{
}

#include <Application.h>
#include <device-settings/DebugLevel.h>
#include <device-settings/EncoderAcceleration.h>
#include <device-settings/Settings.h>
#include <proxies/hwui/panel-unit/RotaryEncoder.h>
#include <testing/TestDriver.h>
#include <proxies/lpc/LPCProxy.h>
#include <proxies/hwui/Oleds.h>
#include <tools/PerformanceTimer.h>
#include <string.h>
#include <nltools/messaging/Messaging.h>
#include <nltools/messaging/Message.h>

static TestDriver<RotaryEncoder> tester;

RotaryEncoder::RotaryEncoder()
    : m_throttler(std::chrono::milliseconds(2))
{
  nltools::msg::receive<nltools::msg::RotaryChangedMessage>(nltools::msg::EndPoint::Playground,
                                                            sigc::mem_fun(this, &RotaryEncoder::onMessage));
}

RotaryEncoder::~RotaryEncoder()
{
  m_stress.disconnect();
}

void RotaryEncoder::onMessage(const nltools::msg::RotaryChangedMessage &msg)
{
  applyIncrement(msg.increment);
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

sigc::connection RotaryEncoder::onRotaryChanged(slot<void, tIncrement> slot)
{
  return m_signalRotaryChanged.connect(slot);
}

void RotaryEncoder::registerTests()
{
}

#include <proxies/hwui/FineButton.h>

FineButton::FineButton()
    : m_shifted(false)
{
  registerTraversal(TOGGLED_OFF, DEPRESSED, TOGGLED_ON);
  registerTraversal(TOGGLED_ON, TIME_ELAPSED, CURRENT_ON);
  registerTraversal(TOGGLED_ON, RELEASED, TOGGLED_ON);
  registerTraversal(TOGGLED_ON, DEPRESSED, TOGGLED_ON_SWITCHING_OFF);
  registerTraversal(TOGGLED_ON_SWITCHING_OFF, RELEASED, TOGGLED_OFF);
  registerTraversal(CURRENT_ON, RELEASED, TOGGLED_OFF);
}

FineButton::~FineButton()
{
}

void FineButton::setButtonState(bool pressed)
{
  m_timeout.disconnect();

  if(pressed)
  {
    traverse(DEPRESSED);
    m_timeout = MainContext::get_default()->signal_timeout().connect(sigc::mem_fun(this, &FineButton::onTimeout), 250);
  }
  else
  {
    traverse(RELEASED);
  }
}

bool FineButton::setState(FineButtonStates s)
{
  if(s == INVALID)
    s = TOGGLED_OFF;

  return super::setState(s);
}

bool FineButton::onTimeout()
{
  traverse(TIME_ELAPSED);
  return false;
}

bool FineButton::getModifierState() const
{
  return getState() != TOGGLED_OFF;
}

bool FineButton::onlyTemporary()
{
  return m_shifted == false;
}

void FineButton::setShiftedWhilePressDown(bool down)
{
  m_shifted = down;
}

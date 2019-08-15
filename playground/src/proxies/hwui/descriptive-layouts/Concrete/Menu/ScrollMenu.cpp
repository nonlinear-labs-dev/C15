#include "ScrollMenu.h"

ScrollMenu::ScrollMenu()
    : ControlWithChildren({ 0, 0, 256, 64 })
{
}

void ScrollMenu::scroll(int direction)
{
  for(auto& c : getControls())
  {
    auto r = c->getPosition();
    r.setTop(r.getTop() + direction);
    c->setPosition(r);
  }

  for(auto& c : getControls<BasicItem>())
  {
    if(c->getPosition().contains(32, 32))
    {
      m_selectedControl = c.get();
      c->setHighlight(true);
    }
    else
    {
      c->setHighlight(false);
    }
  }
}

bool ScrollMenu::onButton(Buttons i, bool down, ButtonModifiers mod)
{
  if(down)
  {
    if(i == Buttons::BUTTON_INC)
    {
      scroll(15);
      return true;
    }
    else if(i == Buttons::BUTTON_DEC)
    {
      scroll(-15);
      return true;
    }
  }

  if(m_selectedControl)
  {
    if(auto rec = dynamic_cast<ButtonReceiver*>(m_selectedControl))
      if(rec->onButton(i, down, mod))
        return true;
  }

  return false;
}

#include "ButtonColumn.h"

#ifdef _DEVELOPMENT_PC

ButtonColumn::ButtonColumn(int firstButtonId)
{

  for(int i = 0; i < 4; i++)
    m_buttons[i] = new Button(firstButtonId + i);

  for(auto &m_button : m_buttons)
    add(*m_button);
}

ButtonColumn::~ButtonColumn()
{
  for(auto &b : m_buttons)
    delete b;
}

void ButtonColumn::setLed(int idx, bool state)
{
  for(auto &p : m_buttons)
    p->setLed(idx, state);
}

#endif

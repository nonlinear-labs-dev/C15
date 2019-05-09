#include "ButtonColumn.h"

#ifdef _DEVELOPMENT_PC

<<<<<<< HEAD
ButtonColumn::ButtonColumn(int firstButtonId)
{

  for(int i = 0; i < 4; i++)
    m_buttons[i] = new Button(firstButtonId + i);

  for(auto &m_button : m_buttons)
    add(*m_button);
=======
ButtonColumn::ButtonColumn(int firstButtonId) :
    m_buttons { firstButtonId, firstButtonId + 1, firstButtonId + 2, firstButtonId + 3 }
{
  for(auto &p : m_buttons)
    add(p);
>>>>>>> layouts-reloaded
}

ButtonColumn::~ButtonColumn()
{
<<<<<<< HEAD
  for(auto &b : m_buttons)
    delete b;
=======
>>>>>>> layouts-reloaded
}

void ButtonColumn::setLed(int idx, bool state)
{
  for(auto &p : m_buttons)
<<<<<<< HEAD
    p->setLed(idx, state);
=======
    p.setLed(idx, state);
>>>>>>> layouts-reloaded
}

#endif

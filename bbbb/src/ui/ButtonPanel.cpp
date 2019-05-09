#include "ButtonPanel.h"

#ifdef _DEVELOPMENT_PC

<<<<<<< HEAD
ButtonPanel::ButtonPanel(int firstButtonId)

{
  for(int i = 0; i < 6; i++)
    m_columns[i] = new ButtonColumn(firstButtonId + (i * 4));

=======
ButtonPanel::ButtonPanel(int firstButtonId) :
    m_columns { firstButtonId, firstButtonId + 4, firstButtonId + 8, firstButtonId + 12, firstButtonId + 16, firstButtonId + 20 }
{
>>>>>>> layouts-reloaded
  set_margin_left(10);
  set_margin_right(10);
  set_margin_top(10);
  set_margin_bottom(10);

  for(auto &p : m_columns)
<<<<<<< HEAD
    add(*p);
=======
    add(p);
>>>>>>> layouts-reloaded
}

ButtonPanel::~ButtonPanel()
{
<<<<<<< HEAD
  for(auto b : m_columns)
    delete b;
=======
>>>>>>> layouts-reloaded
}

void ButtonPanel::setLed(int idx, bool state)
{
  for(auto &p : m_columns)
<<<<<<< HEAD
    p->setLed(idx, state);
=======
    p.setLed(idx, state);
>>>>>>> layouts-reloaded
}

#endif

#include "ButtonPanel.h"

#ifdef _DEVELOPMENT_PC

ButtonPanel::ButtonPanel(int firstButtonId)

{
  for(int i = 0; i < 6; i++)
    m_columns[i] = new ButtonColumn(firstButtonId + (i * 4));

  set_margin_left(10);
  set_margin_right(10);
  set_margin_top(10);
  set_margin_bottom(10);

  for(auto &p : m_columns)
    add(*p);
}

ButtonPanel::~ButtonPanel()
{
  for(auto b : m_columns)
    delete b;
}

void ButtonPanel::setLed(int idx, bool state)
{
  for(auto &p : m_columns)
    p->setLed(idx, state);
}

#endif

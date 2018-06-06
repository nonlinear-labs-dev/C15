#include "ButtonPanel.h"

#ifdef _DEVELOPMENT_PC

ButtonPanel::ButtonPanel(int firstButtonId) :
    m_columns { firstButtonId, firstButtonId + 4, firstButtonId + 8, firstButtonId + 12, firstButtonId + 16, firstButtonId + 20 }
{
  set_margin_left(10);
  set_margin_right(10);
  set_margin_top(10);
  set_margin_bottom(10);

  for(auto &p : m_columns)
    add(p);
}

ButtonPanel::~ButtonPanel()
{
}

void ButtonPanel::setLed(int idx, bool state)
{
  for(auto &p : m_columns)
    p.setLed(idx, state);
}

#endif

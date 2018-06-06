#include "ButtonColumn.h"

#ifdef _DEVELOPMENT_PC

ButtonColumn::ButtonColumn(int firstButtonId) :
    m_buttons { firstButtonId, firstButtonId + 1, firstButtonId + 2, firstButtonId + 3 }
{
  for(auto &p : m_buttons)
    add(p);
}

ButtonColumn::~ButtonColumn()
{
}

void ButtonColumn::setLed(int idx, bool state)
{
  for(auto &p : m_buttons)
    p.setLed(idx, state);
}

#endif

#include "PanelUnit.h"

#ifdef _DEVELOPMENT_PC

PanelUnit::PanelUnit()
{
  int v[4] = { 0, 24, 48, 72 };

  for(int i = 0; i < 4; i++)
    m_panels[i] = new ButtonPanel(v[i]);

  for(auto &p : m_panels)
    add(*p);
}

PanelUnit::~PanelUnit()
{
  for(auto p : m_panels)
    delete p;
}

void PanelUnit::setLed(int idx, bool state)
{
  for(auto &p : m_panels)
    p->setLed(idx, state);
}

#endif

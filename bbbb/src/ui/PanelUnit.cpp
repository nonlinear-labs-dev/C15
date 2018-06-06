#include "PanelUnit.h"

#ifdef _DEVELOPMENT_PC

PanelUnit::PanelUnit() :
    m_panels { 0, 24, 48, 72 }
{
  for(auto &p : m_panels)
    add(p);
}

PanelUnit::~PanelUnit()
{
}

void PanelUnit::setLed(int idx, bool state)
{
  for(auto &p : m_panels)
    p.setLed(idx, state);
}

#endif

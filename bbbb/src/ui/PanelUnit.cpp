#include "PanelUnit.h"

#ifdef _DEVELOPMENT_PC

<<<<<<< HEAD
PanelUnit::PanelUnit()
{
  int v[4] = { 0, 24, 48, 72 };

  for(int i = 0; i < 4; i++)
    m_panels[i] = new ButtonPanel(v[i]);

  for(auto &p : m_panels)
    add(*p);
=======
PanelUnit::PanelUnit() :
    m_panels { 0, 24, 48, 72 }
{
  for(auto &p : m_panels)
    add(p);
>>>>>>> layouts-reloaded
}

PanelUnit::~PanelUnit()
{
<<<<<<< HEAD
  for(auto p : m_panels)
    delete p;
=======
>>>>>>> layouts-reloaded
}

void PanelUnit::setLed(int idx, bool state)
{
  for(auto &p : m_panels)
<<<<<<< HEAD
    p->setLed(idx, state);
=======
    p.setLed(idx, state);
>>>>>>> layouts-reloaded
}

#endif

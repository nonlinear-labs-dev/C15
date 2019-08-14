#include "PanelUnit.h"

#ifdef _DEVELOPMENT_PC

PanelUnit::PanelUnit()
{
  for(auto v: { 0, 24, 48, 72 }) {
    m_panels.emplace_back(std::make_unique<ButtonPanel>(v));
    add(*m_panels.back().get());
  }
}

void PanelUnit::setLed(int idx, bool state)
{
  for(auto &p : m_panels)
    p->setLed(idx, state);
}

#endif

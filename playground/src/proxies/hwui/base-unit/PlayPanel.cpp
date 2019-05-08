#include <glib.h>
#include "PlayPanel.h"

PlayPanel::PlayPanel()
{
}

PlayPanel::~PlayPanel()
{
}

void PlayPanel::onTimeout()
{
  m_oled.redraw();
}

SOLED &PlayPanel::getSOLED()
{
  return m_oled;
}

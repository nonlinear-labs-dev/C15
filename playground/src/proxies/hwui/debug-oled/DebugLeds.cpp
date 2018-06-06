#include "DebugLeds.h"

#include "DebugLedPanel.h"

DebugLeds::DebugLeds (OLEDProxy &oled)
{
  const int screenWidth = 256;
  const int screenHeight = 64;
  const int numPanels = 4;
  const int numLedsPerPanel = 24;

  const int panelWidth = screenWidth / numPanels;
  const int panelHeight = screenHeight;

  int ledID = 0;

  for (int i = 0; i < numPanels; i++)
  {
    Rect r (i * panelWidth, 0, panelWidth, panelHeight);
    addControl (new DebugLedPanel (r, ledID));
    ledID += numLedsPerPanel;
  }
}

DebugLeds::~DebugLeds ()
{
}

void DebugLeds::setLedState (int ledId, bool onOrOff)
{
  for (auto c : getControls ())
    if (auto led = dynamic_pointer_cast < DebugLedPanel > (c))
      if (led->setLedState (ledId, onOrOff))
        return;
}

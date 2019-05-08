#include "DebugLedPanel.h"

#include "DebugLed.h"

DebugLedPanel::DebugLedPanel(const Rect &pos, int startID)
    : super(pos)
{
  const int numRows = 6;
  const int numCols = 4;
  const int ledWidth = pos.getWidth() / numRows;
  const int ledHeight = pos.getHeight() / numCols;

  int ledID = startID;

  for(int row = 0; row < numRows; row++)
  {
    for(int col = 0; col < numCols; col++)
    {
      Rect r(row * ledWidth, col * ledHeight, ledWidth, ledHeight);
      addControl(new DebugLed(r, ledID));
      ledID++;
    }
  }
}

DebugLedPanel::~DebugLedPanel()
{
}

bool DebugLedPanel::setLedState(int ledId, bool onOrOff)
{
  for(auto c : getControls())
    if(auto led = dynamic_pointer_cast<DebugLed>(c))
      if(led->getId() == ledId)
      {
        led->setLedState(onOrOff);
        return true;
      }

  return false;
}

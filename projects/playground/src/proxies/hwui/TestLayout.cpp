#include "TestLayout.h"
#include "proxies/hwui/controls/Rect.h"
#include "proxies/hwui/controls/Control.h"
#include <Application.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/panel-unit/PanelUnit.h>
#include <proxies/hwui/panel-unit/EditPanel.h>
#include <proxies/hwui/FrameBuffer.h>

TestLayout::TestLayout(OLEDProxy &parent)
    : super(parent)
{
}

void TestLayout::iterate()
{
  m_test++;
  setDirty();

  if(m_test >= NumTests)
    getOLEDProxy().resetOverlay();
}

bool TestLayout::redraw(FrameBuffer &fb)
{
  FrameBuffer &s = getFrameBuffer();
  auto r = getOLEDProxy().getPosInFrameBuffer();

  s.setColor(FrameBuffer::Colors::C43);
  s.fillRect(Rect(0, 0, r.getWidth(), r.getHeight()));

  typedef FrameBuffer::tPixel tPixel;

  switch(m_test)
  {
    case Tests::Full:
      s.setColor(FrameBuffer::Colors::C255);
      s.fillRect(Rect(0, 0, r.getWidth(), r.getHeight()));
      break;

    case Tests::Gradient:
      for(int i = 0; i < r.getHeight(); i++)
      {
        float normalized = i / (r.getHeight() - 1.0);
        s.fiddleColor(s.interpolateColor(normalized));
        s.drawHorizontalLine(0, i, r.getWidth());
      }
      break;

    case Tests::Border:
      s.setColor(FrameBuffer::Colors::C255);
      s.drawRect(Rect(0, 0, r.getWidth(), r.getHeight()));
      break;
  }
  return true;
}
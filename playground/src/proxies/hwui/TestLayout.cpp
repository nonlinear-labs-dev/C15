#include "TestLayout.h"
#include "proxies/hwui/controls/Rect.h"
#include "proxies/hwui/controls/Control.h"
#include <Application.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/panel-unit/PanelUnit.h>
#include <proxies/hwui/panel-unit/EditPanel.h>

TestLayout::TestLayout (OLEDProxy &parent)
{
}

TestLayout::~TestLayout ()
{
}

void TestLayout::iterate()
{
  m_test++;
  m_isDirty = true;

}

bool TestLayout::redrawLayout (OLEDProxy& oled)
{
  if (!m_isDirty)
    return false;

  return true;
}

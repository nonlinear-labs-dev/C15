#include <Application.h>
#include <groups/HardwareSourcesGroup.h>
#include <proxies/hwui/base-unit/BaseUnit.h>
#include <proxies/hwui/base-unit/PlayPanel.h>
#include <proxies/hwui/base-unit/soled/MCRoutings.h>
#include <proxies/hwui/base-unit/soled/NoteShiftDisplay.h>
#include <proxies/hwui/base-unit/soled/PlayModeRibbonBehaviourLabel.h>
#include <proxies/hwui/base-unit/soled/PlayOrEditModeLayout.h>
#include <proxies/hwui/base-unit/soled/RibbonLabel.h>
#include <proxies/hwui/controls/ControlOwner.h>
#include <proxies/hwui/controls/DottedLine.h>
#include <proxies/hwui/controls/Rect.h>
#include <proxies/hwui/controls/VerticalLine.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/lpc/LPCProxy.h>


PlayOrEditModeLayout::PlayOrEditModeLayout ()
{
  addControl (new Label ("Shift", Rect (1, 1, 23, 14)));
  addControl (new NoteShiftDisplay (Rect (1, 17, 23, 14)));
  addControl (new VerticalLine (Rect (24, 0, 1, 32)));
  addControl (new DottedLine (Rect (25, 16, 103, 1)));

  m_upperArrow = addControl (new Label ("<", Rect (123, 1, 5, 14)));
  m_lowerArrow = addControl (new Label ("<", Rect (123, 17, 5, 14)));

  addControl (new RibbonLabel (HardwareSourcesGroup::getLowerRibbonParameterID (), Rect (25, 17, 74, 14)));
  addControl (new MCRoutings (HardwareSourcesGroup::getLowerRibbonParameterID (), Rect (99, 20, 8, 8)));
  addControl (new PlayModeRibbonBehaviourLabel (HardwareSourcesGroup::getLowerRibbonParameterID (), Rect (getBehaviourLeft (), 17, 12, 14)));

  Application::get ().getLPCProxy ()->onRibbonTouched (mem_fun (this, &PlayOrEditModeLayout::onLastTouchedRibbonChanged));
}

PlayOrEditModeLayout::~PlayOrEditModeLayout ()
{
}

int PlayOrEditModeLayout::getBehaviourLeft () const
{
  return 109;
}

void PlayOrEditModeLayout::onLastTouchedRibbonChanged (int lastTouchedRibbonParameterID)
{
  bool upper = (lastTouchedRibbonParameterID == HardwareSourcesGroup::getUpperRibbonParameterID ());
  m_upperArrow->setVisible (upper);
  m_lowerArrow->setVisible (!upper);
}

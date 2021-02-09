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
#include <proxies/playcontroller/PlaycontrollerProxy.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameter_declarations.h>

PlayOrEditModeLayout::PlayOrEditModeLayout()
    : super(Application::get().getHWUI()->getBaseUnit().getPlayPanel().getSOLED())
{
  addControl(new Label("Shift", Rect(1, 1, 23, 14)));
  addControl(new NoteShiftDisplay(Rect(1, 17, 23, 14)));
  addControl(new VerticalLine(Rect(24, 0, 1, 32)));
  addControl(new DottedLine(Rect(25, 16, 103, 1)));

  m_upperArrow = addControl(new Label("<", Rect(123, 1, 5, 14)));
  m_lowerArrow = addControl(new Label("<", Rect(123, 17, 5, 14)));

  addControl(new RibbonLabel(HardwareSourcesGroup::getLowerRibbonParameterID(), Rect(25, 17, 74, 14)));
  addControl(new MCRoutings(HardwareSourcesGroup::getLowerRibbonParameterID(), Rect(99, 20, 8, 8)));
  addControl(new PlayModeRibbonBehaviourLabel(HardwareSourcesGroup::getLowerRibbonParameterID(),
                                              Rect(getBehaviourLeft(), 17, 12, 14)));

  Application::get().getPlaycontrollerProxy()->onRibbonTouched(
      mem_fun(this, &PlayOrEditModeLayout::onLastTouchedRibbonChanged));
  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      mem_fun(this, &PlayOrEditModeLayout::onParameterSelectionChanged), {});
}

PlayOrEditModeLayout::~PlayOrEditModeLayout()
{
}

int PlayOrEditModeLayout::getBehaviourLeft() const
{
  return 109;
}

void PlayOrEditModeLayout::onParameterSelectionChanged(const Parameter* old, Parameter* newP, SignalOrigin signalType)
{
  if(newP->getID().getNumber() == C15::PID::Ribbon_1)
  {
    onLastTouchedRibbonChanged(C15::PID::Ribbon_1);
  }
  else if(newP->getID().getNumber() == C15::PID::Ribbon_2)
  {
    onLastTouchedRibbonChanged(C15::PID::Ribbon_2);
  }
}

void PlayOrEditModeLayout::onLastTouchedRibbonChanged(int lastTouchedRibbonParameterID)
{
  bool upper = (lastTouchedRibbonParameterID == HardwareSourcesGroup::getUpperRibbonParameterID().getNumber());
  m_upperArrow->setVisible(upper);
  m_lowerArrow->setVisible(!upper);
}

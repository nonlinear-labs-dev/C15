#include "UnmodulatebaleParameterLayouts.h"
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/SelectedParameterBarSlider.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/SelectedParameterKnubbelSlider.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ParameterNameLabel.h>
#include <proxies/hwui/controls/Button.h>
#include <proxies/hwui/controls/SelectedParameterValue.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ParameterEditButtonMenu.h>

UnmodulateableParameterLayout2::UnmodulateableParameterLayout2()
    : super()
{
}

void UnmodulateableParameterLayout2::init()
{
  super::init();
  addButtons();
}

void UnmodulateableParameterLayout2::addButtons()
{
  addControl(new Button("", Buttons::BUTTON_A));
  addControl(new Button("", Buttons::BUTTON_B));
  addControl(new Button("", Buttons::BUTTON_C));
}

UnmodulateableParameterSelectLayout2::UnmodulateableParameterSelectLayout2()
    : virtual_base()
    , super1()
    , super2()
{
}

void UnmodulateableParameterSelectLayout2::init()
{
  super1::init();
  super2::init();

  if(auto p = getCurrentParameter())
  {
    if(p->getVisualizationStyle() == Parameter::VisualizationStyle::Dot)
      addControl(new SelectedParameterKnubbelSlider(Rect(BIG_SLIDER_X, 24, BIG_SLIDER_WIDTH, 6)));
    else
      addControl(new SelectedParameterBarSlider(Rect(BIG_SLIDER_X, 24, BIG_SLIDER_WIDTH, 6)));
  }

  addControl(new SelectedParameterValue(Rect(90, 33, 76, 12)));

  highlight<SelectedParameterBarSlider>();
  highlight<SelectedParameterValue>();
  highlight<ParameterNameLabel>();
}

UnmodulateableParameterEditLayout2::UnmodulateableParameterEditLayout2()
    : virtual_base()
    , super1()
    , super2()
{
}

void UnmodulateableParameterEditLayout2::init()
{
  super1::init();
  super2::init();

  if(auto p = getCurrentParameter())
  {
    if(p->getVisualizationStyle() == Parameter::VisualizationStyle::Dot)
      addControl(new SelectedParameterKnubbelSlider(Rect(BIG_SLIDER_X, 24, BIG_SLIDER_WIDTH, 6)));
    else
      addControl(new SelectedParameterBarSlider(Rect(BIG_SLIDER_X, 24, BIG_SLIDER_WIDTH, 6)));
  }

  addControl(new SelectedParameterValue(Rect(90, 33, 76, 12)));

  highlight<SelectedParameterBarSlider>();
  highlight<SelectedParameterValue>();
  highlight<ParameterNameLabel>();
}

ButtonMenu *UnmodulateableParameterEditLayout2::createMenu(const Rect &rect)
{
  return new ParameterEditButtonMenu(rect);
}

bool UnmodulateableParameterEditLayout2::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  if(super1::onButton(i, down, modifiers))
    return true;

  return super2::onButton(i, down, modifiers);
}

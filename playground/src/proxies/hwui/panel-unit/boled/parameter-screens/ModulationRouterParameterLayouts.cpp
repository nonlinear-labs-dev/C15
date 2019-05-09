#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/SelectedParameterBarSlider.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ParameterNameLabel.h>
#include <proxies/hwui/controls/Button.h>
#include <proxies/hwui/controls/SelectedParameterValue.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameters/ModulationRoutingParameter.h>
#include <parameters/PhysicalControlParameter.h>
#include <parameters/MacroControlParameter.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ModulationRouterParameterLayouts.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ParameterEditButtonMenu.h>

ModulationRouterParameterSelectLayout2::ModulationRouterParameterSelectLayout2()
    : super()
{
  addControl(new Button("HW Source..", Buttons::BUTTON_A));
  addControl(new Button("", Buttons::BUTTON_B));
  addControl(new Button("", Buttons::BUTTON_C));
  addControl(new Button("MC..", Buttons::BUTTON_D));

  addControl(new SelectedParameterBarSlider(Rect(BIG_SLIDER_X, 24, BIG_SLIDER_WIDTH, 6)));
  addControl(new SelectedParameterValue(Rect(90, 33, 76, 12)));

  highlight<ParameterNameLabel>();
  highlight<SelectedParameterBarSlider>();
  highlight<SelectedParameterValue>();
}

Carousel *ModulationRouterParameterSelectLayout2::createCarousel(const Rect &rect)
{
  return nullptr;
}

bool ModulationRouterParameterSelectLayout2::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  if(down)
  {
    if(i == Buttons::BUTTON_A)
    {
      if(auto p = dynamic_cast<ModulationRoutingParameter *>(getCurrentParameter()))
      {
        Application::get().getPresetManager()->getEditBuffer()->undoableSelectParameter(p->getSourceParameter());
      }

      return true;
    }

    if(i == Buttons::BUTTON_D)
    {
      if(auto p = dynamic_cast<ModulationRoutingParameter *>(getCurrentParameter()))
      {
        p->getSourceParameter()->setUiSelectedModulationRouter(p->getID());

        Application::get().getPresetManager()->getEditBuffer()->undoableSelectParameter(p->getTargetParameter());
      }

      return true;
    }
  }

  return super::onButton(i, down, modifiers);
}

ModulationRouterParameterEditLayout2::ModulationRouterParameterEditLayout2()
    : super()
{
  addControl(new Button("", Buttons::BUTTON_A));
  addControl(new Button("", Buttons::BUTTON_B));
  addControl(new Button("", Buttons::BUTTON_C));

  addControl(new SelectedParameterBarSlider(Rect(BIG_SLIDER_X, 24, BIG_SLIDER_WIDTH, 6)));
  addControl(new SelectedParameterValue(Rect(90, 33, 76, 12)));

  highlight<ParameterNameLabel>();
  highlight<SelectedParameterBarSlider>();
  highlight<SelectedParameterValue>();
}

ButtonMenu *ModulationRouterParameterEditLayout2::createMenu(const Rect &rect)
{
  return new ParameterEditButtonMenu(rect);
}

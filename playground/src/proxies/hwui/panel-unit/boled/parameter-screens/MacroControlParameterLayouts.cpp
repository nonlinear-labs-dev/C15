#include <Application.h>
#include <groups/MacroControlMappingGroup.h>
#include <parameters/MacroControlParameter.h>
#include <parameters/ModulationRoutingParameter.h>
#include <parameters/Parameter.h>
#include <parameters/PhysicalControlParameter.h>
#include <presets/EditBuffer.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/buttons.h>
#include <proxies/hwui/controls/Button.h>
#include <proxies/hwui/controls/ControlOwner.h>
#include <proxies/hwui/controls/DottedLine.h>
#include <proxies/hwui/controls/Overlay.h>
#include <proxies/hwui/controls/Rect.h>
#include <proxies/hwui/controls/SelectedParameterValue.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/HWSourceAmountCarousel.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MacroControlEditButtonMenu.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulationSourceLabel.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ParameterNameLabel.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/SelectedMacroControlsHWSourceAmount.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/SelectedMacroControlsHWSourceName.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/SelectedMacroControlsHWSourceSlider.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/SelectedMacroControlsHWSourceValue.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/SelectedParameterDotSlider.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/MacroControlParameterLayouts.h>
#include <memory>

MacroControlParameterLayout2::MacroControlParameterLayout2()
    : super()
{
  addControl(new SelectedParameterDotSlider(Rect(BIG_SLIDER_X, 24, BIG_SLIDER_WIDTH, 6)));
  addControl(new ModulationSourceLabel(Rect(77, 33, 11, 12)));
  m_modeOverlay = addControl(new Overlay(Rect(0, 0, 256, 64)));

  setMode(Mode::MacroControlValue);
}

void MacroControlParameterLayout2::copyFrom(Layout *other)
{
  if(auto p = dynamic_cast<MacroControlParameterLayout2 *>(other))
    setMode(p->m_mode);

  super::copyFrom(other);
}

Parameter *MacroControlParameterLayout2::getCurrentEditParameter() const
{
  switch(m_mode)
  {
    case Mode::PlayControlSelection:
    case Mode::PlayControlAmount:
      return getCurrentRouter();

    case Mode::MacroControlValue:
      return getCurrentParameter();

    case Mode::PlayControlPosition:
      return getCurrentPlayControl();
  }

  return super::getCurrentEditParameter();
}

Parameter *MacroControlParameterLayout2::getCurrentRouter() const
{
  if(auto editBuffer = Application::get().getPresetManager()->getEditBuffer())
  {
    if(auto tgtParam = dynamic_cast<MacroControlParameter *>(getCurrentParameter()))
    {
      int selectedHWSource = tgtParam->getUiSelectedHardwareSource();

      if(auto srcParam = editBuffer->findParameterByID(selectedHWSource))
      {
        auto csGroup = static_cast<MacroControlMappingGroup *>(editBuffer->getParameterGroupByID("MCM"));
        auto routers = csGroup->getModulationRoutingParametersFor(tgtParam);

        for(auto router : routers)
        {
          if(router->routes(dynamic_cast<const PhysicalControlParameter *>(srcParam)))
          {
            return router;
          }
        }
      }
    }
  }

  return nullptr;
}

Parameter *MacroControlParameterLayout2::getCurrentPlayControl() const
{
  if(auto p = dynamic_cast<MacroControlParameter *>(getCurrentParameter()))
  {
    int selectedHWSource = p->getUiSelectedHardwareSource();
    auto editBuffer = Application::get().getPresetManager()->getEditBuffer();
    return editBuffer->findParameterByID(selectedHWSource);
  }

  return nullptr;
}

bool MacroControlParameterLayout2::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  if(down)
  {
    switch(i)
    {
      case Buttons::BUTTON_A:
        toggleMode(Mode::PlayControlPosition);
        return true;

      case Buttons::BUTTON_B:
      case Buttons::BUTTON_D:
        toggleMode(Mode::PlayControlSelection);
        return true;

      case Buttons::BUTTON_C:
        toggleMode(Mode::PlayControlAmount);
        return true;
    }
  }

  return super::onButton(i, down, modifiers);
}

bool MacroControlParameterLayout2::onRotary(int inc, ButtonModifiers modifiers)
{
  if(m_mode == Mode::PlayControlSelection)
  {
    if(auto p = dynamic_cast<MacroControlParameter *>(getCurrentParameter()))
      p->toggleUiSelectedHardwareSource(inc);

    return true;
  }

  return super::onRotary(inc, modifiers);
}

void MacroControlParameterLayout2::toggleMode(Mode desiredMode)
{
  if(m_mode == desiredMode)
    setMode(Mode::MacroControlValue);
  else
    setMode(desiredMode);
}

MacroControlParameterLayout2::Mode MacroControlParameterLayout2::getMode() const
{
  return m_mode;
}

void MacroControlParameterLayout2::setMode(Mode desiredMode)
{
  m_mode = desiredMode;

  m_modeOverlay->clear();
  noHighlight();
  setDirty();

  switch(m_mode)
  {
    case Mode::MacroControlValue:
      m_modeOverlay->addControl(new SelectedParameterValue(Rect(90, 33, 76, 12)));

      highlight<ParameterNameLabel>();
      highlight<HWSourceAmountCarousel>();
      highlight<SelectedParameterDotSlider>();
      m_modeOverlay->highlight<SelectedParameterValue>();
      break;

    case Mode::PlayControlPosition:
      m_modeOverlay->addControl(new SelectedMacroControlsHWSourceSlider(Rect(8, 25, 48, 4)));
      m_modeOverlay->addControl(new SelectedMacroControlsHWSourceValue(Rect(0, BUTTON_VALUE_Y_POSITION, 64, 12)));
      m_modeOverlay->addControl(new SelectedMacroControlsHWSourceName(Rect(64, BUTTON_VALUE_Y_POSITION, 64, 12)));
      m_modeOverlay->addControl(new SelectedMacroControlsHWSourceAmount(Rect(131, BUTTON_VALUE_Y_POSITION, 58, 12)));
      m_modeOverlay->addControl(new DottedLine(Rect(60, 27, 13, 1)));

      highlightButtonWithCaption("HW Pos");
      m_modeOverlay->highlight<SelectedMacroControlsHWSourceSlider>();
      m_modeOverlay->highlight<SelectedMacroControlsHWSourceValue>();

      findControlOfType<HWSourceAmountCarousel>()->highlightSelected();
      break;

    case Mode::PlayControlSelection:
      m_modeOverlay->addControl(new SelectedMacroControlsHWSourceSlider(Rect(8, 25, 48, 4)));
      m_modeOverlay->addControl(new SelectedMacroControlsHWSourceValue(Rect(0, BUTTON_VALUE_Y_POSITION, 64, 12)));
      m_modeOverlay->addControl(new SelectedMacroControlsHWSourceName(Rect(64, BUTTON_VALUE_Y_POSITION, 64, 12)))
          ->setHighlight(true);
      m_modeOverlay->addControl(new SelectedMacroControlsHWSourceAmount(Rect(131, BUTTON_VALUE_Y_POSITION, 58, 12)));
      m_modeOverlay->addControl(new DottedLine(Rect(60, 27, 13, 1)));
      highlightButtonWithCaption("HW Sel");
      findControlOfType<HWSourceAmountCarousel>()->highlightSelected();
      break;

    case Mode::PlayControlAmount:
      m_modeOverlay->addControl(new SelectedParameterDotSlider(Rect(BIG_SLIDER_X, 24, BIG_SLIDER_WIDTH, 6)));
      m_modeOverlay->addControl(new SelectedMacroControlsHWSourceSlider(Rect(8, 25, 48, 4)));
      m_modeOverlay->addControl(new SelectedMacroControlsHWSourceValue(Rect(0, BUTTON_VALUE_Y_POSITION, 64, 12)));
      m_modeOverlay->addControl(new SelectedMacroControlsHWSourceName(Rect(64, BUTTON_VALUE_Y_POSITION, 64, 12)));
      m_modeOverlay->addControl(new SelectedMacroControlsHWSourceAmount(Rect(131, BUTTON_VALUE_Y_POSITION, 58, 12)))
          ->setHighlight(true);
      m_modeOverlay->addControl(new DottedLine(Rect(60, 27, 13, 1)));
      highlightButtonWithCaption("HW Amt");
      findControlOfType<HWSourceAmountCarousel>()->highlightSelected();
      break;
  }
}

MacroControlParameterSelectLayout2::MacroControlParameterSelectLayout2()
    : virtual_base()
    , super1()
    , super2()
{
  addControl(new Button("HW Pos", Buttons::BUTTON_A));
  addControl(new Button("HW Sel", Buttons::BUTTON_B));
  addControl(new Button("HW Amt", Buttons::BUTTON_C));
}

void MacroControlParameterSelectLayout2::init()
{
  super1::init();
  super2::init();
}

Carousel *MacroControlParameterSelectLayout2::createCarousel(const Rect &rect)
{
  return new HWSourceAmountCarousel(Rect(195, 0, 58, 64));
}

bool MacroControlParameterSelectLayout2::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  if(Buttons::BUTTON_D == i && getMode() == Mode::MacroControlValue)
  {
    toggleMode(Mode::PlayControlAmount);
    return true;
  }

  if(super1::onButton(i, down, modifiers))
    return true;

  return super2::onButton(i, down, modifiers);
}

void MacroControlParameterSelectLayout2::setMode(Mode desiredMode)
{
  super2::setMode(desiredMode);
}

MacroControlParameterEditLayout2::MacroControlParameterEditLayout2()
    : virtual_base()
    , super1()
    , super2()
{
  addControl(new Button("", Buttons::BUTTON_A));
  addControl(new Button("", Buttons::BUTTON_B));
  addControl(new Button("", Buttons::BUTTON_C));
}

ButtonMenu *MacroControlParameterEditLayout2::createMenu(const Rect &rect)
{
  return new MacroControlEditButtonMenu(rect);
}

void MacroControlParameterEditLayout2::setMode(Mode desiredMode)
{
  super2::setMode(desiredMode);
  getMenu()->highlightSelectedButton();
}

bool MacroControlParameterEditLayout2::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  if(super1::onButton(i, down, modifiers))
    return true;

  if(i == Buttons::BUTTON_A || i == Buttons::BUTTON_B || i == Buttons::BUTTON_C)
    return true;

  return super2::onButton(i, down, modifiers);
}

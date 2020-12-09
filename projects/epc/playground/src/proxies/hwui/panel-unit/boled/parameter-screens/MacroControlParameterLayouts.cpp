#include <Application.h>
#include <groups/MacroControlMappingGroup.h>
#include <parameters/MacroControlParameter.h>
#include <parameters/ModulationRoutingParameter.h>
#include <parameters/Parameter.h>
#include <parameters/PhysicalControlParameter.h>
#include <presets/EditBuffer.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/buttons.h>
#include <proxies/hwui/HWUI.h>
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
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/VoiceGroupIndicator.h>
#include <sigc++/adaptors/hide.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MCAssignedIndicator.h>

MacroControlParameterLayout2::MacroControlParameterLayout2()
    : super()
{
  addControl(new SelectedParameterDotSlider(Rect(BIG_SLIDER_X, 24, BIG_SLIDER_WIDTH, 6)));
  addControl(new ModulationSourceLabel(Rect(77, 33, 11, 12)));
  m_modeOverlay = addControl(new Overlay(Rect(0, 0, 256, 64)));

  setMode(Mode::MacroControlValue);

  m_editBufferTypeConnection = Application::get().getPresetManager()->getEditBuffer()->onSoundTypeChanged(
      sigc::hide(sigc::mem_fun(this, &MacroControlParameterLayout2::onSoundTypeChanged)));
}

MacroControlParameterLayout2::~MacroControlParameterLayout2()
{
  m_editBufferTypeConnection.disconnect();
}

void MacroControlParameterLayout2::onSoundTypeChanged()
{
  setMode(getMode());
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
      auto selectedHWSource = tgtParam->getUiSelectedHardwareSource();

      if(auto srcParam = editBuffer->findParameterByID(selectedHWSource))
      {
        auto csGroup
            = static_cast<MacroControlMappingGroup *>(editBuffer->getParameterGroupByID({ "MCM", VoiceGroup::Global }));
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
    auto selectedHWSource = p->getUiSelectedHardwareSource();
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
      {
        auto buttonText = getButtonText(Buttons::BUTTON_A);
        if(buttonText == "HW Pos")
        {
          toggleMode(Mode::PlayControlPosition);
        }
        else if(buttonText == "I / II")
        {
          Application::get().getHWUI()->toggleCurrentVoiceGroup();
        }
      }
        return true;

      case Buttons::BUTTON_B:
      case Buttons::BUTTON_D:
        toggleMode(Mode::PlayControlSelection);
        return true;

      case Buttons::BUTTON_C:
        if(getMode() == Mode::MacroControlValue)
          selectSmoothingParameterForMC();
        else
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

void MacroControlParameterLayout2::setButtonA(Button *a)
{
  m_buttonA = a;
}

void MacroControlParameterLayout2::setButtonAText(const std::string &s)
{
  if(m_buttonA)
  {
    m_buttonA->setText({ s, 0 });
  }
}

void MacroControlParameterLayout2::setButtonText(Buttons b, const std::string &s)
{
  for(auto &button : getControls<Button>())
  {
    if(Button::getButtonPos(b) == button->getPosition())
    {
      button->setText({ s, 0 });
    }
  }
}

std::string MacroControlParameterLayout2::getButtonText(Buttons b) const
{
  for(auto &button : getControls<Button>())
  {
    if(Button::getButtonPos(b) == button->getPosition())
    {
      return button->getText().text;
    }
  }
  return "";
}

void MacroControlParameterLayout2::setMode(Mode desiredMode)
{
  m_mode = desiredMode;

  m_modeOverlay->clear();
  noHighlight();
  setDirty();

  if(auto voiceGroupIndication = findControlOfType<VoiceGroupIndicator>())
  {
    voiceGroupIndication->setVisible(desiredMode == Mode::MacroControlValue);
  }

  const auto isDual = Application::get().getPresetManager()->getEditBuffer()->isDual();

  switch(m_mode)
  {
    case Mode::MacroControlValue:
      m_modeOverlay->addControl(createParameterValueControl());
      m_modeOverlay->addControl(createMCAssignmentIndicator());
      setButtonAText(isDual ? "I / II" : "");
      setButtonText(Buttons::BUTTON_B, "HW Sel");
      setButtonText(Buttons::BUTTON_C, "more..");

      highlight<ParameterNameLabel>();
      highlight<HWSourceAmountCarousel>();
      highlight<SelectedParameterDotSlider>();
      m_modeOverlay->highlight<SelectedParameterValue>();
      break;

    case Mode::PlayControlPosition:
      setButtonText(Buttons::BUTTON_A, "HW Pos");
      setButtonText(Buttons::BUTTON_B, "HW Sel");
      setButtonText(Buttons::BUTTON_C, "HW Amt");

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
      setButtonText(Buttons::BUTTON_A, "HW Pos");
      setButtonText(Buttons::BUTTON_B, "HW Sel");
      setButtonText(Buttons::BUTTON_C, "HW Amt");

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
      setButtonText(Buttons::BUTTON_A, "HW Pos");
      setButtonText(Buttons::BUTTON_B, "HW Sel");
      setButtonText(Buttons::BUTTON_C, "HW Amt");

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

void MacroControlParameterLayout2::selectSmoothingParameterForMC()
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  if(auto mc = dynamic_cast<MacroControlParameter *>(getCurrentParameter()))
  {
    eb->undoableSelectParameter(mc->getSmoothingParameter());
  }
}

Control *MacroControlParameterLayout2::createMCAssignmentIndicator()
{
  return new MCAssignedIndicator(Rect(25, 15, 52, 24), getCurrentParameter());
}

MacroControlParameterSelectLayout2::MacroControlParameterSelectLayout2()
    : virtual_base()
    , super1()
    , super2()
{
  setButtonA(addControl(new Button("", Buttons::BUTTON_A)));

  if(Application::get().getPresetManager()->getEditBuffer()->isDual())
  {
    if(getMode() == Mode::MacroControlValue)
    {
      setButtonAText("I / II");
    }
    else
    {
      setButtonAText("");
    }
  }

  addControl(new Button("HW Sel", Buttons::BUTTON_B));
  addControl(new Button("more..", Buttons::BUTTON_C));
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
}

ButtonMenu *MacroControlParameterEditLayout2::createMenu(const Rect &rect)
{
  return new MacroControlEditButtonMenu(rect);
}

void MacroControlParameterEditLayout2::setMode(Mode desiredMode)
{
  super2::setMode(desiredMode);
  getMenu()->highlightSelectedButton();

  for(auto &button : getControls<Button>())
  {
    remove(button.get());
  }

  addControl(new Button("", Buttons::BUTTON_A));
  addControl(new Button("", Buttons::BUTTON_B));
  addControl(new Button("", Buttons::BUTTON_C));

  if(auto vgIndi = findControlOfType<VoiceGroupIndicator>())
    remove(vgIndi.get());
}

Control *MacroControlParameterEditLayout2::createMCAssignmentIndicator()
{
  return nullptr;
}

bool MacroControlParameterEditLayout2::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  if(super1::onButton(i, down, modifiers))
    return true;

  if(i == Buttons::BUTTON_A || i == Buttons::BUTTON_B || i == Buttons::BUTTON_C)
    return true;

  return super2::onButton(i, down, modifiers);
}
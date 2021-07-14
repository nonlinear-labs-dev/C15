#include <Application.h>
#include <glibconfig.h>
#include <glibmm/ustring.h>
#include <groups/MacroControlsGroup.h>
#include <libundo/undo/Scope.h>
#include <libundo/undo/Transaction.h>
#include <libundo/undo/TransactionCreationScope.h>
#include <parameters/ModulateableParameter.h>
#include <parameters/Parameter.h>
#include <proxies/hwui/HWUI.h>
#include <presets/EditBuffer.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/buttons.h>
#include <proxies/hwui/controls/ControlOwner.h>
#include <proxies/hwui/controls/Overlay.h>
#include <proxies/hwui/controls/Rect.h>
#include <proxies/hwui/controls/SelectedParameterValue.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MCAmountButton.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MCAmountLabel.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MCPositionButton.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MCPositionLabel.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MCSelectButton.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulationCarousel.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulationSourceEnabledDottedLine.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulationSourceLabel.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ParameterCarousel.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ParameterNameLabel.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/SelectedParameterBarSlider.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/SelectedParameterKnubbelSlider.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/SelectedParameterModAmount.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/SelectedParamsMacroControlSlider.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ModulateableParameterLayouts.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ParameterEditButtonMenu.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulateableParameterRecallControls/RecallButton.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulateableParameterRecallControls/RecallModulationSourceLabel.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulateableParameterRecallControls/RecallMCPositionLabel.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulateableParameterRecallControls/RecallMCAmountLabel.h>
#include <proxies/hwui/HWUI.h>
#include <parameters/MacroControlParameter.h>
#include <proxies/hwui/controls/SwitchVoiceGroupButton.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/VoiceGroupIndicator.h>
#include "ModulateableParameterLayouts.h"
#include "ModAspectRecallOverlay.h"

ModulateableParameterLayout2::ModulateableParameterLayout2()
{
}

void ModulateableParameterLayout2::addModAmountSliders(ControlOwner *o)
{
  o->addControl(new SelectedParameterModAmount(Rect(BIG_SLIDER_X + 1, 23, BIG_SLIDER_WIDTH - 2, 1)));
  o->addControl(new SelectedParameterModAmount(Rect(BIG_SLIDER_X + 1, 30, BIG_SLIDER_WIDTH - 2, 1)));
}

bool ModulateableParameterLayout2::switchToNormalMode()
{
  return false;
}

ModulateableParameterSelectLayout2::ModulateableParameterSelectLayout2()
    : virtual_base()
    , super1()
    , super2()
{
  m_mcPosButton = addControl(new SwitchVoiceGroupButton(Buttons::BUTTON_A));
  m_mcSelButton = addControl(new MCSelectButton(Buttons::BUTTON_B));
  m_mcAmtButton = addControl(new MCAmountButton(Buttons::BUTTON_C));

  m_modeOverlay = addControl(new Overlay(Rect(0, 0, 256, 64)));

  setMode(Mode::ParameterValue);

  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      sigc::mem_fun(this, &ModulateableParameterSelectLayout2::onSelectedParameterChanged),
      getHWUI()->getCurrentVoiceGroup());

  Application::get().getHWUI()->onModifiersChanged(
      sigc::mem_fun(this, &ModulateableParameterSelectLayout2::onModfiersChanged));
}

void ModulateableParameterSelectLayout2::onSelectedParameterChanged(Parameter *, Parameter *newParam)
{
  m_paramConnection.disconnect();

  if(newParam)
    m_paramConnection = newParam->onParameterChanged(
        sigc::mem_fun(this, &ModulateableParameterSelectLayout2::onCurrentParameterChanged));
}

void ModulateableParameterSelectLayout2::handleSelectPartButton()
{
  if(m_mcPosButton)
  {
    remove(m_mcPosButton);
    m_mcPosButton = nullptr;
  }

  if(!m_mcPosButton)
  {
    if(m_mode == Mode::ParameterValue)
    {
      m_mcPosButton = addControl(new SwitchVoiceGroupButton(Buttons::BUTTON_A));
    }
    else
    {
      m_mcPosButton = addControl(new MCPositionButton(Buttons::BUTTON_A));
    }
  }
}

void ModulateableParameterSelectLayout2::onModfiersChanged(ButtonModifiers modifiers)
{
  auto modParam = dynamic_cast<const ModulateableParameter *>(getCurrentParameter());
  auto inModRecallAbleMode
      = isModeOf({ Mode::MacroControlAmount, Mode::MacroControlPosition, Mode::MacroControlSelection, Mode::Recall });

  if(modParam && inModRecallAbleMode)
  {
    if(modifiers[SHIFT] && (modParam->isAnyModChanged() || modParam->isMacroControlAssignedAndChanged()))
    {
      toggleMode(Mode::Recall);
    }
    else if(!modifiers[SHIFT])
    {
      if(m_mode == Mode::Recall)
      {
        toggleMode(m_lastMode);
      }
    }
  }
}

void ModulateableParameterSelectLayout2::handlePresetValueRecall()
{
  if(m_mode == Mode::ParameterValue)
    ParameterLayout2::handlePresetValueRecall();
}

void ModulateableParameterSelectLayout2::onCurrentParameterChanged(const Parameter *p)
{
  fixModeIfNecessary(p);
  installModulationCarousel(m_mode);
}

void ModulateableParameterSelectLayout2::fixModeIfNecessary(const Parameter *p)
{
  auto isModulateable = dynamic_cast<const ModulateableParameter *>(p) != nullptr;
  auto isModulated
      = isModulateable && dynamic_cast<const ModulateableParameter *>(p)->getModulationSource() != MacroControls::NONE;

  switch(m_mode)
  {
    case Mode::ParameterValue:
      return;

    case Mode::MacroControlPosition:
    case Mode::MacroControlAmount:
    case Mode::CarouselUpperBound:
    case Mode::CarouselParameterValue:
    case Mode::CarouselLowerBound:
      if(!isModulated)
        setMode(Mode::ParameterValue);
      break;

    case Mode::MacroControlSelection:
      if(!isModulateable)
        setMode(Mode::ParameterValue);
      break;
  }
}

void ModulateableParameterSelectLayout2::copyFrom(Layout *other)
{
  if(auto p = dynamic_cast<ModulateableParameterSelectLayout2 *>(other))
    setMode(p->m_mode);

  super1::copyFrom(other);
  super2::copyFrom(other);
}

void ModulateableParameterSelectLayout2::init()
{
  super1::init();
  super2::init();
}

bool ModulateableParameterSelectLayout2::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  if(auto m = dynamic_cast<ModulationCarousel *>(getCarousel()))
  {
    if(m->onButton(i, down, modifiers))
      return true;
  }

  auto modParam = dynamic_cast<const ModulateableParameter *>(getCurrentParameter());

  if(m_modAspectRecallOverlay)
  {
    if(i == Buttons::BUTTON_SHIFT && !down)
    {
      removeModAspectRecall();
    }
    else
    {
      return m_modAspectRecallOverlay->onButton(i, down, modifiers);
    }
  }

  bool isAssigned = false;
  if(modParam)
    isAssigned = modParam->getMacroControl() != nullptr;

  if(down)
  {
    switch(i)
    {
      case Buttons::BUTTON_A:
        if(m_mode == Mode::ParameterValue && !isCurrentParameterDisabled())
        {
          Application::get().getHWUI()->toggleCurrentVoiceGroup();
          return true;
        }
        else if(!isCurrentParameterDisabled() && m_mode != Mode::MacroControlPosition && isAssigned)
        {
          setMode(Mode::MacroControlPosition);
          return true;
        }
        else if(!isCurrentParameterDisabled() && m_mode == Mode::MacroControlPosition)
        {
          setMode(Mode::ParameterValue);
          return true;
        }
        break;

      case Buttons::BUTTON_B:
        if(!isCurrentParameterDisabled())
          toggleMode(Mode::MacroControlSelection);
        return true;

      case Buttons::BUTTON_C:
        if(hasModulationSource() && !isCurrentParameterDisabled())
          toggleMode(Mode::MacroControlAmount);
        return true;

      case Buttons::BUTTON_D:
        if(auto m = dynamic_cast<ModulationCarousel *>(getCarousel()))
        {
          if(m_mode == Mode::CarouselUpperBound)
            toggleMode(Mode::CarouselParameterValue);
          else if(m_mode == Mode::CarouselParameterValue)
            toggleMode(Mode::CarouselLowerBound);
          else
            toggleMode(Mode::CarouselUpperBound);
          return true;
        }
        break;
    }
  }

  if(super1::onButton(i, down, modifiers))
    return true;

  return super2::onButton(i, down, modifiers);
}

Parameter *ModulateableParameterSelectLayout2::getCurrentEditParameter() const
{
  if(m_mode == Mode::MacroControlPosition)
  {
    if(auto p = dynamic_cast<ModulateableParameter *>(getCurrentParameter()))
    {
      auto src = p->getModulationSource();
      auto srcParamID = MacroControlsGroup::modSrcToParamId(src);
      return Application::get().getPresetManager()->getEditBuffer()->findParameterByID(srcParamID);
    }
  }

  return super1::getCurrentEditParameter();
}

bool ModulateableParameterSelectLayout2::hasModulationSource() const
{
  if(auto p = dynamic_cast<ModulateableParameter *>(getCurrentParameter()))
  {
    auto src = p->getModulationSource();
    return src != MacroControls::NONE;
  }
  return false;
}

bool ModulateableParameterSelectLayout2::onRotary(int inc, ButtonModifiers modifiers)
{
  switch(m_mode)
  {
    case Mode::MacroControlSelection:
      if(auto p = dynamic_cast<ModulateableParameter *>(getCurrentParameter()))
      {
        ModParameterUseCases useCase(p);
        useCase.incMCSelection(inc);
      }

      return true;

    case Mode::MacroControlAmount:
      if(auto p = dynamic_cast<ModulateableParameter *>(getCurrentParameter()))
      {
        ModParameterUseCases useCase(p);
        useCase.incModAmount(inc, modifiers[ButtonModifier::FINE]);
      }

      return true;

    case Mode::ParameterValue:
    case Mode::MacroControlPosition:
      break;

    case Mode::CarouselLowerBound:
    case Mode::CarouselUpperBound:
    case Mode::CarouselParameterValue:
      if(auto m = dynamic_cast<ModulationCarousel *>(getCarousel()))
      {
        return m->onRotary(inc, modifiers);
      }
      break;
  }

  if(super1::onRotary(inc, modifiers))
    return true;

  return super2::onRotary(inc, modifiers);
}

void ModulateableParameterSelectLayout2::setDefault()
{
  switch(m_mode)
  {
    case Mode::MacroControlSelection:
      if(auto p = dynamic_cast<ModulateableParameter *>(getCurrentParameter()))
      {
        ModParameterUseCases useCases(p);
        useCases.selectModSource(MacroControls::NONE);
      }

      return;

    case Mode::MacroControlAmount:
      if(auto p = dynamic_cast<ModulateableParameter *>(getCurrentParameter()))
        p->undoableSetMCAmountToDefault();

      return;

    case Mode::ParameterValue:
    case Mode::MacroControlPosition:
      super1::setDefault();
      break;
  }
}

bool ModulateableParameterSelectLayout2::switchToNormalMode()
{
  if(m_mode == Mode::ParameterValue)
    return false;

  setMode(Mode::ParameterValue);
  return true;
}

void ModulateableParameterSelectLayout2::toggleMode(Mode desiredMode)
{
  if(m_mode == desiredMode)
    setMode(Mode::ParameterValue);
  else
    setMode(desiredMode);
}

void ModulateableParameterSelectLayout2::installMcAmountScreen()
{
  setMode(Mode::MacroControlAmount);
}

void ModulateableParameterSelectLayout2::setMode(Mode desiredMode)
{
  auto modp = dynamic_cast<ModulateableParameter *>(getCurrentParameter());

  if(desiredMode == Mode::Recall)
  {
    if(m_mode == Mode::MacroControlSelection && !modp->isModSourceChanged())
      return;
    else if(m_mode == Mode::MacroControlAmount && !modp->isModAmountChanged())
      return;
    else if(m_mode == Mode::MacroControlPosition && !modp->getMacroControl()->isValueChangedFromLoaded())
      return;
  }

  m_lastMode = m_mode;
  m_mode = desiredMode;

  m_modeOverlay->clear();
  m_mcAmtButton->setVisible(true);
  m_mcSelButton->setVisible(true);

  if(auto indication = findControlOfType<VoiceGroupIndicator>())
  {
    indication->setVisible(desiredMode == Mode::ParameterValue);
  }

  if(isCurrentParameterDisabled())
  {
    m_mcPosButton->setText(StringAndSuffix { "" });
    m_mcAmtButton->setText(StringAndSuffix { "" });
    m_mcSelButton->setText(StringAndSuffix { "" });
  }

  handleSelectPartButton();

  m_mcPosButton->setVisible(true);

  noHighlight();
  setDirty();

  if(auto p = getCurrentParameter())
  {
    switch(p->getVisualizationStyle())
    {
      case Parameter::VisualizationStyle::Bar:
      case Parameter::VisualizationStyle::BarFromRight:
        m_modeOverlay->addControl(new SelectedParameterBarSlider(Rect(BIG_SLIDER_X, 24, BIG_SLIDER_WIDTH, 6)));
        break;
      case Parameter::VisualizationStyle::Dot:
        m_modeOverlay->addControl(new SelectedParameterKnubbelSlider(Rect(BIG_SLIDER_X, 24, BIG_SLIDER_WIDTH, 6)));
        break;
    }
  }

  switch(m_mode)
  {
    case Mode::ParameterValue:
      addModAmountSliders(m_modeOverlay);
      m_modeOverlay->addControl(createParameterValueControl());
      m_modeOverlay->addControl(new ModulationSourceLabel(Rect(42, 21, 13, 12), Font::Justification::Right));
      m_modeOverlay->addControl(new ModulationSourceEnabledDottedLine(Rect(57, 27, 17, 1)));

      setCarousel(createCarousel(Rect(195, 0, 58, 62)));

      m_modeOverlay->highlight<SelectedParameterValue>();
      m_modeOverlay->highlight<SelectedParameterBarSlider>();
      m_modeOverlay->highlight<SelectedParameterKnubbelSlider>();
      highlight<ParameterNameLabel>();
      highlight<ParameterCarousel>();
      break;

    case Mode::Recall:
      installModAspectRecall();
      break;

    case Mode::MacroControlPosition:
      addModAmountSliders(m_modeOverlay);
      m_modeOverlay->addControl(new ModulationSourceEnabledDottedLine(Rect(60, 27, 13, 1)));
      m_modeOverlay->addControl(new SelectedParamsMacroControlSlider(Rect(8, 25, 48, 4)));
      m_modeOverlay->addControl(new ModulationSourceLabel(Rect(64, BUTTON_VALUE_Y_POSITION, 64, 12)));
      m_modeOverlay->addControl(new MCPositionLabel(Rect(0, BUTTON_VALUE_Y_POSITION, 64, 12)));
      m_modeOverlay->addControl(new MCAmountLabel(Rect(131, BUTTON_VALUE_Y_POSITION, 58, 12)));

      setCarousel(new ModulationCarousel(ModulationCarousel::Mode::None, Rect(195, 1, 58, 62)));

      m_modeOverlay->highlight<SelectedParamsMacroControlSlider>();
      m_modeOverlay->highlight<MCPositionLabel>();
      highlight<MCPositionButton>();
      break;

    case Mode::MacroControlSelection:
      addModAmountSliders(m_modeOverlay);
      m_modeOverlay->addControl(new ModulationSourceLabel(Rect(64, BUTTON_VALUE_Y_POSITION, 64, 12)));
      m_modeOverlay->addControl(new MCPositionLabel(Rect(0, BUTTON_VALUE_Y_POSITION, 64, 12)));
      m_modeOverlay->addControl(new MCAmountLabel(Rect(131, BUTTON_VALUE_Y_POSITION, 58, 12)));
      m_modeOverlay->addControl(new ModulationSourceEnabledDottedLine(Rect(60, 27, 13, 1)));
      m_modeOverlay->addControl(new SelectedParamsMacroControlSlider(Rect(8, 25, 48, 4)));

      installModulationCarousel(Mode::MacroControlSelection);

      m_modeOverlay->highlight<ModulationSourceLabel>();
      highlight<MCSelectButton>();
      break;

    case Mode::MacroControlAmount:
      addModAmountSliders(m_modeOverlay);
      m_modeOverlay->addControl(new ModulationSourceLabel(Rect(64, BUTTON_VALUE_Y_POSITION, 64, 12)));
      m_modeOverlay->addControl(new MCPositionLabel(Rect(0, BUTTON_VALUE_Y_POSITION, 64, 12)));
      m_modeOverlay->addControl(new MCAmountLabel(Rect(131, BUTTON_VALUE_Y_POSITION, 58, 12)));
      m_modeOverlay->addControl(new ModulationSourceEnabledDottedLine(Rect(60, 27, 13, 1)));
      m_modeOverlay->addControl(new SelectedParamsMacroControlSlider(Rect(8, 25, 48, 4)));

      setCarousel(new ModulationCarousel(ModulationCarousel::Mode::None, Rect(195, 1, 58, 62)));

      m_modeOverlay->highlight<MCAmountLabel>();
      highlight<MCAmountButton>();
      break;

    case Mode::CarouselUpperBound:
      addModAmountSliders(m_modeOverlay);
      m_modeOverlay->addControl(new ModulationSourceLabel(Rect(64, BUTTON_VALUE_Y_POSITION, 64, 12)));
      m_modeOverlay->addControl(new MCPositionLabel(Rect(0, BUTTON_VALUE_Y_POSITION, 64, 12)));
      m_modeOverlay->addControl(new MCAmountLabel(Rect(131, BUTTON_VALUE_Y_POSITION, 58, 12)));
      m_modeOverlay->addControl(new ModulationSourceEnabledDottedLine(Rect(60, 27, 13, 1)));
      m_modeOverlay->addControl(new SelectedParamsMacroControlSlider(Rect(8, 25, 48, 4)));
      setCarousel(new ModulationCarousel(ModulationCarousel::Mode::UpperBound, Rect(195, 1, 58, 62)));
      break;

    case Mode::CarouselParameterValue:
      addModAmountSliders(m_modeOverlay);
      m_modeOverlay->addControl(new ModulationSourceLabel(Rect(64, BUTTON_VALUE_Y_POSITION, 64, 12)));
      m_modeOverlay->addControl(new MCPositionLabel(Rect(0, BUTTON_VALUE_Y_POSITION, 64, 12)));
      m_modeOverlay->addControl(new MCAmountLabel(Rect(131, BUTTON_VALUE_Y_POSITION, 58, 12)));
      m_modeOverlay->addControl(new ModulationSourceEnabledDottedLine(Rect(60, 27, 13, 1)));
      m_modeOverlay->addControl(new SelectedParamsMacroControlSlider(Rect(8, 25, 48, 4)));
      setCarousel(new ModulationCarousel(ModulationCarousel::Mode::ParameterValue, Rect(195, 1, 58, 62)));

      break;

    case Mode::CarouselLowerBound:
      addModAmountSliders(m_modeOverlay);
      m_modeOverlay->addControl(new ModulationSourceLabel(Rect(64, BUTTON_VALUE_Y_POSITION, 64, 12)));
      m_modeOverlay->addControl(new MCPositionLabel(Rect(0, BUTTON_VALUE_Y_POSITION, 64, 12)));
      m_modeOverlay->addControl(new MCAmountLabel(Rect(131, BUTTON_VALUE_Y_POSITION, 58, 12)));
      m_modeOverlay->addControl(new ModulationSourceEnabledDottedLine(Rect(60, 27, 13, 1)));
      m_modeOverlay->addControl(new SelectedParamsMacroControlSlider(Rect(8, 25, 48, 4)));
      setCarousel(new ModulationCarousel(ModulationCarousel::Mode::LowerBound, Rect(195, 1, 58, 62)));
      break;
  }
}

bool ModulateableParameterSelectLayout2::isCurrentParameterDisabled() const
{
  return getCurrentParameter()->isDisabled();
}

bool ModulateableParameterSelectLayout2::isModeOf(std::vector<ModulateableParameterSelectLayout2::Mode> modes) const
{
  for(const auto &mode : modes)
  {
    if(mode == m_mode)
      return true;
  }
  return false;
}

void ModulateableParameterSelectLayout2::installModulationCarousel(const Mode &mode)
{
  if(auto mod = dynamic_cast<ModulateableParameter *>(getCurrentParameter()))
  {
    if(mod->getModulationSource() != MacroControls::NONE && !isModeOf({ Mode::Recall, Mode::ParameterValue }))
    {
      ModulationCarousel::Mode mode = ModulationCarousel::Mode::None;
      if(auto modCarousel = dynamic_cast<ModulationCarousel *>(getCarousel()))
      {
        mode = modCarousel->getMode();
      }

      setCarousel(new ModulationCarousel(mode, Rect(195, 1, 58, 62)));
    }
    else
    {
      setCarousel(createCarousel(Rect(195, 0, 58, 62)));
    }
  }
}

void ModulateableParameterSelectLayout2::installModAspectRecall()
{
  if(m_modAspectRecallOverlay == nullptr)
  {
    if(auto mod = dynamic_cast<ModulateableParameter *>(getCurrentParameter()))
    {
      ModAspectRecallOverlay::Mode openMode;
      switch(m_lastMode)
      {
        default:
        case Mode::MacroControlPosition:
          openMode = ModAspectRecallOverlay::Mode::MC_POS;
          break;
        case Mode::MacroControlAmount:
          openMode = ModAspectRecallOverlay::Mode::MC_AMT;
          break;
        case Mode::MacroControlSelection:
          openMode = ModAspectRecallOverlay::Mode::MC_SEL;
          break;
      }

      m_modAspectRecallOverlay = addControl(new ModAspectRecallOverlay(Rect(0, 0, 256, 64), mod, openMode));
      getCarousel()->setVisible(false);
    }
  }
}

void ModulateableParameterSelectLayout2::removeModAspectRecall()
{
  if(m_modAspectRecallOverlay)
  {
    remove(m_modAspectRecallOverlay);
    getCarousel()->setVisible(true);
    m_modAspectRecallOverlay = nullptr;

    cleanMode();
  }
}

void ModulateableParameterSelectLayout2::cleanMode()
{
  auto modAspectFocus = isModeOf({ Mode::MacroControlPosition, Mode::MacroControlAmount });
  auto mcAssigned = dynamic_cast<ModulateableParameter *>(getCurrentParameter())->getMacroControl() != nullptr;

  if(modAspectFocus && !mcAssigned)
  {
    setMode(Mode::ParameterValue);
  }
}

ModulateableParameterSelectLayout2::Mode ModulateableParameterSelectLayout2::getMode() const
{
  return m_mode;
}

ModulateableParameterEditLayout2::ModulateableParameterEditLayout2()
    : super1()
    , super2()
{
  addControl(new Button("", Buttons::BUTTON_A));
  addControl(new Button("", Buttons::BUTTON_B));
  addControl(new Button("", Buttons::BUTTON_C));

  if(auto p = getCurrentParameter())
  {
    switch(p->getVisualizationStyle())
    {
      case Parameter::VisualizationStyle::Bar:
      case Parameter::VisualizationStyle::BarFromRight:
        addControl(new SelectedParameterBarSlider(Rect(BIG_SLIDER_X, 24, BIG_SLIDER_WIDTH, 6)));
        break;
      case Parameter::VisualizationStyle::Dot:
        addControl(new SelectedParameterKnubbelSlider(Rect(BIG_SLIDER_X, 24, BIG_SLIDER_WIDTH, 6)));
        break;
    }
  }

  addModAmountSliders(this);
  addControl(createParameterValueControl());

  highlight<SelectedParameterValue>();
  highlight<SelectedParameterBarSlider>();
  highlight<SelectedParameterKnubbelSlider>();
  highlight<ParameterNameLabel>();

  if(auto switchVG = findControlOfType<SwitchVoiceGroupButton>())
  {
    remove(switchVG.get());
  }
}

ButtonMenu *ModulateableParameterEditLayout2::createMenu(const Rect &rect)
{
  return new ParameterEditButtonMenu(rect);
}

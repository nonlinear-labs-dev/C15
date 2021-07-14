#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/SelectedParameterModAmount.h>
#include <proxies/hwui/controls/Button.h>
#include <parameters/MacroControlParameter.h>
#include <presets/recall/RecallParameter.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulateableParameterRecallControls/RecallMCPositionLabel.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MCPositionLabel.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulateableParameterRecallControls/RecallMCAmountLabel.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MCAmountLabel.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulationSourceLabel.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulateableParameterRecallControls/RecallModulationSourceLabel.h>
#include "ModAspectRecallOverlay.h"
#include <proxies/hwui/FrameBuffer.h>
#include <groups/MacroControlsGroup.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameters/scale-converters/MacroControlScaleConverter.h>

bool ModAspectRecallOverlay::onButton(Buttons i, bool down, ButtonModifiers mod)
{
  if(!down)
    return false;

  if(i == Buttons::BUTTON_D)
  {
    nextMode();
    return true;
  }
  if(i == Buttons::BUTTON_A)
  {
    previousMode();
    return true;
  }

  auto& recall = m_oldRecallValues[m_mode];
  if(i == Buttons::BUTTON_B && !recall.likeInPreset)
  {
    doRecall();
    return true;
  }

  if(i == Buttons::BUTTON_C && recall.likeInPreset)
  {
    undoRecall();
    return true;
  }

  return false;
}

ModAspectRecallOverlay::ModAspectRecallOverlay(const Rect& r, ModulateableParameter* modP, Mode openMode)
    : Overlay { r }
    , m_modParam { modP }
    , m_mode { openMode }
{
  constexpr static int BIG_SLIDER_X = 77;
  constexpr static int BIG_SLIDER_WIDTH = 102;

  addControl(new SelectedParameterModAmount(Rect(BIG_SLIDER_X + 1, 23, BIG_SLIDER_WIDTH - 2, 1)));
  addControl(new SelectedParameterModAmount(Rect(BIG_SLIDER_X + 1, 30, BIG_SLIDER_WIDTH - 2, 1)));

  m_labelA = addControl(new Label({ 0, 35, 64, 12 }));
  m_labelB = addControl(new Label({ 64, 35, 64, 12 }));
  m_labelC = addControl(new Label({ 128, 35, 64, 12 }));

  m_buttonA = addControl(new Button("<", Buttons::BUTTON_A));
  m_buttonB = addControl(new Button("", Buttons::BUTTON_B));
  m_buttonC = addControl(new Button("", Buttons::BUTTON_C));
  m_buttonD = addControl(new Button(">", Buttons::BUTTON_D));

  m_labelC->setHighlight(true);

  m_oldRecallValues[Mode::MC_POS] = { isMCAssignedAndChanged(), false, getOGMCPos(), getMCPos() };
  m_oldRecallValues[Mode::MC_SEL] = { isMCSelChanged(), false, getOGMCSelection(), getMCSelection() };
  m_oldRecallValues[Mode::MC_AMT] = { isMCAmtChanged(), false, getOGAmount(), getMCAmount() };

  installMode(openMode);
}

int ModAspectRecallOverlay::getZPriority() const
{
  return 1;
}

bool ModAspectRecallOverlay::redraw(FrameBuffer& fb)
{
  drawBackground(fb);
  return ControlWithChildren::redraw(fb);
}

void ModAspectRecallOverlay::drawBackground(FrameBuffer& fb)
{
  fb.setColor(FrameBufferColors::C43);
  auto pos = getPosition();
  pos.setLeft(pos.getRight() - 64);
  fb.fillRect(pos);
}

void ModAspectRecallOverlay::installMode(ModAspectRecallOverlay::Mode m)
{
  m_mode = m;
  updateUI();
}

void ModAspectRecallOverlay::doRecall()
{
  auto& recall = m_oldRecallValues[m_mode];
  recall.anyRecallHappened = true;

  ModParameterUseCases useCase(m_modParam);

  switch(m_mode)
  {
    case Mode::MC_SEL:
      recall.rightRecallValue = m_modParam->getModulationSource();
      useCase.recallMCSource();
      recall.leftRecallValue = m_modParam->getModulationSource();
      break;
    case Mode::MC_AMT:
      recall.rightRecallValue = m_modParam->getModulationAmount();
      useCase.recallMCAmount();
      recall.leftRecallValue = m_modParam->getModulationAmount();
      break;
    case Mode::MC_POS:
      recall.rightRecallValue = m_modParam->getMacroControl()->getControlPositionValue();
      useCase.recallMCPos();
      recall.leftRecallValue = m_modParam->getMacroControl()->getControlPositionValue();
      break;
  }

  recall.likeInPreset = true;
  updateUI();
}

void ModAspectRecallOverlay::undoRecall()
{
  ModParameterUseCases useCase(m_modParam);
  auto& recall = m_oldRecallValues[m_mode];
  auto old = recall.rightRecallValue;
  recall.anyRecallHappened = true;

  switch(m_mode)
  {
    case Mode::MC_SEL:
      recall.leftRecallValue = m_modParam->getModulationSource();
      useCase.undoRecallMCSel(std::get<MacroControls>(old));
      recall.rightRecallValue = m_modParam->getModulationSource();
      break;
    case Mode::MC_AMT:
      recall.leftRecallValue = m_modParam->getModulationAmount();
      useCase.undoRecallMCAmt(std::get<double>(old));
      recall.rightRecallValue = m_modParam->getModulationAmount();
      break;
    case Mode::MC_POS:
      recall.leftRecallValue = m_modParam->getMacroControl()->getControlPositionValue();
      useCase.undoRecallMCPos(std::get<double>(old));
      recall.rightRecallValue = m_modParam->getMacroControl()->getControlPositionValue();
      break;
  }

  recall.likeInPreset = false;
  updateUI();
}

void ModAspectRecallOverlay::previousMode()
{
  switch(m_mode)
  {
    case MC_POS:
      if(isMCAmtChanged() || m_oldRecallValues[MC_AMT].anyRecallHappened)
        installMode(MC_AMT);
      else if(isMCSelChanged() || m_oldRecallValues[MC_SEL].anyRecallHappened)
        installMode(MC_SEL);
      break;
    case MC_SEL:
      if(isMCAssignedAndChanged() || m_oldRecallValues[MC_POS].anyRecallHappened)
        installMode(MC_POS);
      else if(isMCAmtChanged() || m_oldRecallValues[MC_AMT].anyRecallHappened)
        installMode(MC_AMT);
      break;
    case MC_AMT:
      if(isMCSelChanged() || m_oldRecallValues[MC_SEL].anyRecallHappened)
        installMode(MC_SEL);
      else if(isMCAssignedAndChanged() || m_oldRecallValues[MC_POS].anyRecallHappened)
        installMode(MC_POS);
      break;
  }
}

void ModAspectRecallOverlay::nextMode()
{
  switch(m_mode)
  {
    case MC_POS:
      if(isMCSelChanged() || m_oldRecallValues[MC_SEL].anyRecallHappened)
        installMode(MC_SEL);
      else if(isMCAmtChanged() || m_oldRecallValues[MC_AMT].anyRecallHappened)
        installMode(MC_AMT);
      break;
    case MC_SEL:
      if(isMCAmtChanged() || m_oldRecallValues[MC_AMT].anyRecallHappened)
        installMode(MC_AMT);
      else if(isMCAssignedAndChanged() || m_oldRecallValues[MC_SEL].anyRecallHappened)
        installMode(MC_POS);
      break;
    case MC_AMT:
      if(isMCAssignedAndChanged() || m_oldRecallValues[MC_POS].anyRecallHappened)
        installMode(MC_POS);
      else if(isMCSelChanged() || m_oldRecallValues[MC_SEL].anyRecallHappened)
        installMode(MC_SEL);
      break;
  }
}

void ModAspectRecallOverlay::updateUI()
{
  auto& recall = m_oldRecallValues[m_mode];

  auto arrowsEnabled = false;

  switch(m_mode)
  {
    case MC_POS:
      arrowsEnabled = isMCAmtChanged() || isMCSelChanged() || m_oldRecallValues[MC_AMT].anyRecallHappened
          || m_oldRecallValues[MC_SEL].anyRecallHappened;
      m_labelA->setText(StringAndSuffix { "MC Position" });
      m_labelB->setText(StringAndSuffix { stringizeMCPos(std::get<double>(recall.leftRecallValue)), 0 });
      m_labelC->setText(StringAndSuffix { stringizeMCPos(std::get<double>(recall.rightRecallValue)), 0 });
      break;
    case MC_SEL:
      arrowsEnabled = isMCAmtChanged() || isMCAssignedAndChanged() || m_oldRecallValues[MC_AMT].anyRecallHappened
          || m_oldRecallValues[MC_POS].anyRecallHappened;
      m_labelA->setText(StringAndSuffix { "MC Selection" });
      m_labelB->setText(StringAndSuffix { mcSelectionToDisplay(std::get<MacroControls>(recall.leftRecallValue)), 0 });
      m_labelC->setText(StringAndSuffix { mcSelectionToDisplay(std::get<MacroControls>(recall.rightRecallValue)), 0 });
      break;
    case MC_AMT:
      arrowsEnabled = isMCAssignedAndChanged() || isMCSelChanged() || m_oldRecallValues[MC_SEL].anyRecallHappened
          || m_oldRecallValues[MC_POS].anyRecallHappened;
      m_labelA->setText(StringAndSuffix { "MC Amount" });
      m_labelB->setText(
          StringAndSuffix { m_modParam->stringizeModulationAmount(std::get<double>(recall.leftRecallValue)), 0 });
      m_labelC->setText(
          StringAndSuffix { m_modParam->stringizeModulationAmount(std::get<double>(recall.rightRecallValue)), 0 });
      break;
  }

  m_labelB->setHighlight(recall.likeInPreset);
  m_labelC->setHighlight(!recall.likeInPreset);

  m_buttonB->setText(StringAndSuffix { recall.likeInPreset ? "" : "Recall" });
  m_buttonC->setText(StringAndSuffix { recall.likeInPreset ? "Recall" : "" });

  m_buttonA->setText(StringAndSuffix { arrowsEnabled ? "<" : "" });
  m_buttonD->setText(StringAndSuffix { arrowsEnabled ? ">" : "" });
}

//Helper

std::string ModAspectRecallOverlay::mcSelectionToDisplay(MacroControls src) const
{
  auto id = MacroControlsGroup::modSrcToParamId(src);
  if(auto mc = Application::get().getPresetManager()->getEditBuffer()->findParameterByID(id))
  {
    return mc->getShortName();
  }
  return "[-]";
}

std::string ModAspectRecallOverlay::stringizeMCPos(tDisplayValue src) const
{
  auto id = MacroControlsGroup::modSrcToParamId(MacroControls::MC1);
  if(auto mc = Application::get().getPresetManager()->getEditBuffer()->findParameterByID(id))
  {
    return mc->getDisplayString(src);
  }
  return std::string();
}

bool ModAspectRecallOverlay::isMCAssignedAndChanged() const
{
  if(auto mc = m_modParam->getMacroControl())
  {
    return mc->isValueChangedFromLoaded();
  }
  return false;
}

bool ModAspectRecallOverlay::isMCAmtChanged() const
{
  return m_modParam->isModAmountChanged();
}

bool ModAspectRecallOverlay::isMCSelChanged() const
{
  return m_modParam->isModSourceChanged();
}

float ModAspectRecallOverlay::getOGAmount() const
{
  if(auto og = m_modParam->getOriginalParameter())
  {
    return og->getRecallModulationAmount();
  }
  return {};
}

float ModAspectRecallOverlay::getOGMCPos() const
{
  if(auto mc = m_modParam->getMacroControl())
  {
    if(auto og = mc->getOriginalParameter())
    {
      return og->getRecallValue();
    }
  }
  return {};
}

MacroControls ModAspectRecallOverlay::getOGMCSelection() const
{
  if(auto og = m_modParam->getOriginalParameter())
  {
    return og->getRecallModSource();
  }
  return MacroControls::NONE;
}

float ModAspectRecallOverlay::getMCAmount() const
{
  return m_modParam->getModulationAmount();
}

float ModAspectRecallOverlay::getMCPos() const
{
  if(auto mc = m_modParam->getMacroControl())
  {
    return mc->getControlPositionValue();
  }
  return {};
}

MacroControls ModAspectRecallOverlay::getMCSelection() const
{
  return m_modParam->getModulationSource();
}
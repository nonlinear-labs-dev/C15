#include "ModParameterUseCases.h"
#include <presets/EditBuffer.h>
#include <parameters/ModulateableParameter.h>
#include <parameters/MacroControlParameter.h>
#include <presets/recall/RecallParameter.h>
#include <nltools/Assert.h>
#include <libundo/undo/Scope.h>
#include <Application.h>
#include <parameters/scale-converters/ScaleConverter.h>
#include <presets/PresetManager.h>
#include <cmath>

ModParameterUseCases::ModParameterUseCases(ModulateableParameter* parameter)
    : m_modParam { parameter }
{
  nltools_assertAlways(m_modParam != nullptr);
}

void ModParameterUseCases::recallMCPos()
{
  if(auto mc = m_modParam->getMacroControl())
  {
    ParameterUseCases useCase(mc);
    useCase.recallParameterFromPreset();
    m_modParam->onChange(UpdateDocumentContributor::ChangeFlags::Generic);
  }
}

void ModParameterUseCases::recallMCAmount()
{
  if(!m_modParam->isModAmountChanged())
    return;

  auto& scope = m_modParam->getUndoScope();
  if(auto original = m_modParam->getOriginalParameter())
  {

    auto name = m_modParam->getGroupAndParameterName();
    auto transactionScope = scope.startTransaction("Recall MC Amount from Preset for '%0'", name);
    auto transaction = transactionScope->getTransaction();
    m_modParam->setModulationAmount(transaction, original->getRecallModulationAmount());
  }
}

void ModParameterUseCases::recallMCSource()
{
  if(!m_modParam->isModSourceChanged())
    return;

  if(auto original = m_modParam->getOriginalParameter())
  {
    auto& scope = m_modParam->getUndoScope();
    auto name = m_modParam->getGroupAndParameterName();
    auto transactionScope = scope.startTransaction("Recall MC Selection from Preset for '%0'", name);
    auto transaction = transactionScope->getTransaction();
    if(original)
    {
      m_modParam->setModulationSource(transaction, original->getRecallModSource());
      m_modParam->onChange(UpdateDocumentContributor::ChangeFlags::Generic);
    }
  }
}

void ModParameterUseCases::incMCSelection(int incs)
{
  //TODO: is it really clean to get the soundtype here?
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  auto dual = m_modParam->getID().isDual() && eb->isDual();
  auto name = dual ? m_modParam->getGroupAndParameterNameWithVoiceGroup() : m_modParam->getGroupAndParameterName();
  auto scope = m_modParam->getUndoScope().startTransaction("Set MC Select for '%0'", name);

  auto src = (int) m_modParam->getModulationSource();
  auto numChoices = static_cast<int>(MacroControls::NUM_CHOICES);
  src += incs;

  while(src < 0)
    src += numChoices;

  while(src >= numChoices)
    src -= numChoices;

  m_modParam->setModulationSource(scope->getTransaction(), (MacroControls) src);
}

void ModParameterUseCases::incModAmount(int incs, bool fine)
{
  //TODO clean same as above: inject Editbuffer instead of singleton grabbing it
  auto dual = m_modParam->getID().isDual() && Application::get().getPresetManager()->getEditBuffer()->isDual();

  auto cookie = m_modParam->getAmountCookie();
  auto name = dual ? m_modParam->getGroupAndParameterNameWithVoiceGroup() : m_modParam->getGroupAndParameterName();
  auto scope = m_modParam->getUndoScope().startContinuousTransaction(cookie, "Set MC Amount for '%0'", name);

  tDisplayValue controlVal = m_modParam->getModulationAmount();
  double denominator
      = fine ? m_modParam->getModulationAmountFineDenominator() : m_modParam->getModulationAmountCoarseDenominator();
  int rasterized = static_cast<int>(std::round(controlVal * denominator));

  controlVal = ScaleConverter::getControlPositionRangeBipolar().clip((rasterized + incs) / denominator);
  m_modParam->setModulationAmount(scope->getTransaction(), controlVal);
}

void ModParameterUseCases::selectModSource(MacroControls mc)
{
  //TODO remove editbuffer dependency
  auto isDual = Application::get().getPresetManager()->getEditBuffer()->isDual() && m_modParam->getID().isDual();
  auto scope = m_modParam->getUndoScope().startTransaction("Set MC Select for '%0'",
                                                           isDual ? m_modParam->getGroupAndParameterNameWithVoiceGroup()
                                                                  : m_modParam->getGroupAndParameterName());
  m_modParam->setModulationSource(scope->getTransaction(), mc);
}

void ModParameterUseCases::undoRecallMCSel(MacroControls& controls)
{
  auto name = m_modParam->getGroupAndParameterName();
  auto scope = m_modParam->getUndoScope().startTransaction("Recall MC Selection from Editbuffer for '%0'", name);
  m_modParam->setModulationSource(scope->getTransaction(), controls);
}

void ModParameterUseCases::undoRecallMCAmt(tControlPositionValue mcAmt)
{
  auto name = m_modParam->getGroupAndParameterName();
  auto scope = m_modParam->getUndoScope().startTransaction("Recall MC Amount from Editbuffer for '%0'", name);
  m_modParam->setModulationAmount(scope->getTransaction(), mcAmt);
}

void ModParameterUseCases::undoRecallMCPos(tControlPositionValue mcPos)
{
  auto name = m_modParam->getGroupAndParameterName();
  auto scope = m_modParam->getUndoScope().startTransaction("Recall assigned MC Pos from Editbuffer for '%0'", name);
  m_modParam->getMacroControl()->setCPFromHwui(scope->getTransaction(), mcPos);
}

void ModParameterUseCases::removeModSource()
{
  auto scope = m_modParam->getUndoScope().startTransaction("Remove Modulation Source");
  m_modParam->undoableSelectModSource(scope->getTransaction(), MacroControls::NONE);
}

void ModParameterUseCases::selectModSourceAndSelectTargetParameter(MacroControls mc)
{
  auto scope = m_modParam->getUndoScope().startTransaction("Set Modulation Source");
  m_modParam->undoableSelectModSource(scope->getTransaction(), mc);

  if(auto eb = dynamic_cast<EditBuffer*>(m_modParam->getParentGroup()->getParent()))
    eb->undoableSelectParameter(scope->getTransaction(), m_modParam, SignalOrigin::EXPLICIT);
}

void ModParameterUseCases::setModulationAmount(tControlPositionValue amt)
{
  //TODO remove EB dependency
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  auto dual = eb->isDual() && m_modParam->getID().isDual();
  auto name = dual ? m_modParam->getGroupAndParameterNameWithVoiceGroup() : m_modParam->getGroupAndParameterName();
  auto scope = m_modParam->getUndoScope().startContinuousTransaction(m_modParam->getAmountCookie(),
                                                                     "Set MC Amount for '%0'", name);
  m_modParam->setModulationAmount(scope->getTransaction(), amt);
}

void ModParameterUseCases::setModulationLimit(tControlPositionValue amt, tControlPositionValue cp)
{
  auto& undoScope = m_modParam->getUndoScope();
  const auto name = m_modParam->getGroupAndParameterName();
  auto scope = undoScope.startContinuousTransaction(m_modParam, "Set Modulation Limit for '%0'", name);
  auto transaction = scope->getTransaction();
  m_modParam->undoableSetModAmount(transaction, amt);
  m_modParam->setCPFromHwui(transaction, cp);
}

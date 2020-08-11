#include "ModulateableParameter.h"
#include "MacroControlParameter.h"
#include "groups/MacroControlsGroup.h"
#include "proxies/lpc/MessageComposer.h"
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterInfoLayout.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ModulateableParameterLayouts.h>
#include "parameters/scale-converters/Linear100PercentScaleConverter.h"
#include "scale-converters/LinearBipolar100PercentScaleConverter.h"
#include <libundo/undo/Transaction.h>
#include <device-settings/DebugLevel.h>
#include <xml/Writer.h>
#include <presets/ParameterGroupSet.h>
#include <presets/PresetParameter.h>
#include <Application.h>
#include <presets/EditBuffer.h>
#include <presets/Preset.h>
#include <testing/TestRootDocument.h>
#include <proxies/audio-engine/AudioEngineProxy.h>
#include <parameters/messaging/ParameterMessageFactory.h>
#include <presets/recall/RecallParameter.h>
#include <proxies/hwui/HWUI.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <http/UndoScope.h>

ModulateableParameter::ModulateableParameter(ParameterGroup *group, ParameterId id, const ScaleConverter *scaling,
                                             tDisplayValue def, tControlPositionValue coarseDenominator,
                                             tControlPositionValue fineDenominator)
    : Parameter(group, id, scaling, def, coarseDenominator, fineDenominator)
    , m_modulationAmount(0)
    , m_modSource(MacroControls::NONE)
{
}

ModulateableParameter::~ModulateableParameter() = default;

size_t ModulateableParameter::getHash() const
{
  size_t hash = super::getHash();
  hash_combine(hash, m_modulationAmount);
  hash_combine(hash, (int) m_modSource);
  return hash;
}

tDisplayValue ModulateableParameter::getModulationAmount() const
{
  return m_modulationAmount;
}

void ModulateableParameter::writeToLPC(MessageComposer &cmp) const
{
  Parameter::writeToLPC(cmp);
  cmp << getModulationSourceAndAmountPacked();
}

uint16_t ModulateableParameter::getModulationSourceAndAmountPacked() const
{
  if(getModulationSource() == MacroControls::NONE)
    return 0;

  auto scaled = static_cast<gint16>(round(m_modulationAmount * getModulationAmountFineDenominator()));
  auto abs = (scaled < 0) ? -scaled : scaled;
  auto src = static_cast<gint16>(getModulationSource());

  g_assert(src > 0);
  src--;

  auto sign = static_cast<gint16>((scaled < 0) ? 1 : 0);
  auto toSend = static_cast<uint16_t>((src << 14) | (sign << 13) | (abs));

  return toSend;
}

template <typename T> T clamp(T v, T min, T max)
{
  return std::max<T>(min, std::min<T>(v, max));
}

void ModulateableParameter::setModulationAmount(UNDO::Transaction *transaction, const tDisplayValue &amount)
{
  auto clampedAmount = clamp<tDisplayValue>(amount, -1.0, 1.0);

  if(m_modulationAmount != clampedAmount)
  {
    auto swapData = UNDO::createSwapData(clampedAmount);

    transaction->addSimpleCommand([=](UNDO::Command::State) mutable {
      swapData->swapWith(m_modulationAmount);
      getValue().resetSaturation();
      DebugLevel::gassy("mod amount set to", m_modulationAmount);
      invalidate();
      sendToLpc();
    });
  }
}

void ModulateableParameter::sendParameterMessage() const
{
  if(Application::exists())
    Application::get().getAudioEngineProxy()->createAndSendParameterMessage<ModulateableParameter>(this);
}

MacroControls ModulateableParameter::getModulationSource() const
{
  return m_modSource;
}

void ModulateableParameter::copyFrom(UNDO::Transaction *transaction, const PresetParameter *other)
{
  if(!isLocked())
  {
    super::copyFrom(transaction, other);
    setModulationSource(transaction, other->getModulationSource());
    setModulationAmount(transaction, other->getModulationAmount());
  }
}

void ModulateableParameter::copyTo(UNDO::Transaction *transaction, PresetParameter *other) const
{
  super::copyTo(transaction, other);
  other->setField(transaction, PresetParameter::Fields::ModSource, to_string(getModulationSource()));
  other->setField(transaction, PresetParameter::Fields::ModAmount, to_string(getModulationAmount()));
}

void ModulateableParameter::setModulationSource(UNDO::Transaction *transaction, MacroControls src)
{
  if(m_modSource != src)
  {
    auto swapData = UNDO::createSwapData(src);

    transaction->addSimpleCommand([=](UNDO::Command::State) mutable {
      if(auto groups = dynamic_cast<ParameterGroupSet *>(getParentGroup()->getParent()))
      {
        if(m_modSource != MacroControls::NONE)
        {
          auto modSrc = dynamic_cast<MacroControlParameter *>(
              groups->findParameterByID(MacroControlsGroup::modSrcToParamId(m_modSource)));
          modSrc->unregisterTarget(this);
        }

        swapData->swapWith(m_modSource);

        if(m_modSource != MacroControls::NONE)
        {
          auto modSrc = dynamic_cast<MacroControlParameter *>(
              groups->findParameterByID(MacroControlsGroup::modSrcToParamId(m_modSource)));
          modSrc->registerTarget(this);
        }

        getValue().resetSaturation();
        sendToLpc();
      }
      else
      {
        swapData->swapWith(m_modSource);
        getValue().resetSaturation();
      }

      invalidate();
    });
  }
}

void ModulateableParameter::applyLpcMacroControl(tDisplayValue diff)
{
  if(isBiPolar())
    diff *= 2;

  getValue().changeRawValue(Initiator::EXPLICIT_LPC, diff * m_modulationAmount);
}

void ModulateableParameter::undoableSelectModSource(UNDO::Transaction *transaction, MacroControls src)
{
  setModulationSource(transaction, src);
}

void ModulateableParameter::undoableSetModAmount(UNDO::Transaction *transaction, double amount)
{
  setModulationAmount(transaction, amount);
}

void *ModulateableParameter::getAmountCookie()
{
  return &m_modulationAmount;
}

void ModulateableParameter::undoableSetMCAmountToDefault()
{
  tDisplayValue def = 0.0;

  if(m_modulationAmount != def)
  {
    auto eb = Application::get().getPresetManager()->getEditBuffer();
    auto dual = eb->isDual() && getID().isDual();
    auto scope = getUndoScope().startContinuousTransaction(getAmountCookie(), "Set MC Amount for '%0'",
                                                           dual ? getGroupAndParameterNameWithVoiceGroup()
                                                                : getGroupAndParameterName());
    setModulationAmount(scope->getTransaction(), def);
  }
}

void ModulateableParameter::undoableIncrementMCSelect(int inc)
{
  auto dual = getID().isDual() && Application::get().getPresetManager()->getEditBuffer()->isDual();
  auto scope = getUndoScope().startTransaction(
      "Set MC Select for '%0'", dual ? getGroupAndParameterNameWithVoiceGroup() : getGroupAndParameterName());
  undoableIncrementMCSelect(scope->getTransaction(), inc);
}

void ModulateableParameter::undoableIncrementMCSelect(UNDO::Transaction *transaction, int inc)
{
  auto src = (int) getModulationSource();
  auto numChoices = static_cast<int>(MacroControls::NUM_CHOICES);
  src += inc;

  while(src < 0)
    src += numChoices;

  while(src >= numChoices)
    src -= numChoices;

  setModulationSource(transaction, (MacroControls) src);
}

void ModulateableParameter::undoableIncrementMCAmount(int inc, ButtonModifiers modifiers)
{
  auto dual = getID().isDual() && Application::get().getPresetManager()->getEditBuffer()->isDual();

  auto scope = getUndoScope().startContinuousTransaction(getAmountCookie(), "Set MC Amount for '%0'",
                                                         dual ? getGroupAndParameterNameWithVoiceGroup()
                                                              : getGroupAndParameterName());
  undoableIncrementMCAmount(scope->getTransaction(), inc, modifiers);
}

void ModulateableParameter::undoableIncrementMCAmount(UNDO::Transaction *transaction, int inc,
                                                      ButtonModifiers modifiers)
{
  tDisplayValue controlVal = getModulationAmount();
  double denominator = getModAmountDenominator(modifiers);
  int rasterized = static_cast<int>(round(controlVal * denominator));
  controlVal = ScaleConverter::getControlPositionRangeBipolar().clip((rasterized + inc) / denominator);
  setModulationAmount(transaction, controlVal);
}

int ModulateableParameter::getModAmountDenominator(const ButtonModifiers &modifiers) const
{
  auto denom = modifiers[FINE] ? getModulationAmountFineDenominator() : getModulationAmountCoarseDenominator();
  return static_cast<int>(denom);
}

void ModulateableParameter::writeDocProperties(Writer &writer, tUpdateID knownRevision) const
{
  Parameter::writeDocProperties(writer, knownRevision);

  writer.writeTextElement("modAmount", to_string(m_modulationAmount));
  writer.writeTextElement("modSrc", to_string(static_cast<int>(m_modSource)));

  if(shouldWriteDocProperties(knownRevision))
  {
    writer.writeTextElement("mod-amount-coarse", to_string(getModulationAmountCoarseDenominator()));
    writer.writeTextElement("mod-amount-fine", to_string(getModulationAmountFineDenominator()));
  }
}

void ModulateableParameter::loadDefault(UNDO::Transaction *transaction)
{
  undoableSelectModSource(transaction, MacroControls::NONE);
  undoableSetModAmount(transaction, 0.0);
  super::loadDefault(transaction);
}

void ModulateableParameter::undoableLoadPackedModulationInfo(UNDO::Transaction *transaction,
                                                             const Glib::ustring &packedModulationInfo)
{
  auto bits = std::stoul(packedModulationInfo);
  auto modSrc = (bits & 0xC000) >> 14;
  auto modAmount = bits & 0x1FFF;
  auto negative = bits & 0x2000;

  if(negative && modAmount == 0)
  {
    undoableSelectModSource(transaction, MacroControls::NONE);
    undoableSetModAmount(transaction, 0.0);
  }
  else
  {
    auto iModSrc = static_cast<MacroControls>(modSrc + 1);
    undoableSelectModSource(transaction, iModSrc);

    auto fModAmount = (negative ? -1.0 : 1.0) * modAmount / getModulationAmountFineDenominator();
    undoableSetModAmount(transaction, fModAmount);
  }
}

double ModulateableParameter::getModulationAmountFineDenominator() const
{
  auto fineDenominator = getValue().getFineDenominator();

  while(fineDenominator > 8000)
    fineDenominator /= 2;

  return fineDenominator;
}

double ModulateableParameter::getModulationAmountCoarseDenominator() const
{
  return getValue().getCoarseDenominator();
}

void ModulateableParameter::exportReaktorParameter(std::stringstream &target) const
{
  super::exportReaktorParameter(target);
  auto packedModulationInfo = getModulationSourceAndAmountPacked();

  if(m_modSource == MacroControls::NONE)
    packedModulationInfo = 0x2000;

  target << packedModulationInfo << std::endl;
}

Glib::ustring ModulateableParameter::stringizeModulationAmount() const
{
  return stringizeModulationAmount(getModulationAmount());
}

Glib::ustring ModulateableParameter::stringizeModulationAmount(tControlPositionValue amount) const
{
  LinearBipolar100PercentScaleConverter converter;
  return converter.getDimension().stringize(converter.controlPositionToDisplay(amount));
}

Layout *ModulateableParameter::createLayout(FocusAndMode focusAndMode) const
{
  switch(focusAndMode.mode)
  {
    case UIMode::Info:
      return new ParameterInfoLayout();

    case UIMode::Edit:
      return new ModulateableParameterEditLayout2();

    case UIMode::Select:
    default:
      return new ModulateableParameterSelectLayout2();
  }

  g_return_val_if_reached(nullptr);
}

std::pair<tControlPositionValue, tControlPositionValue> ModulateableParameter::getModulationRange(bool clipped) const
{
  double modLeft = 0;
  double modRight = 0;

  auto src = getModulationSource();
  if(src != MacroControls::NONE)
  {
    auto srcParamID = MacroControlsGroup::modSrcToParamId(src);
    auto editBuffer = dynamic_cast<const EditBuffer *>(getParentGroup()->getParent());

    if(editBuffer)
    {
      if(auto srcParam = editBuffer->findParameterByID(srcParamID))
      {
        auto modAmount = getModulationAmount();
        auto srcValue = srcParam->getValue().getRawValue();
        auto value = getValue().getRawValue();

        if(isBiPolar())
          modLeft = 0.5 * (value + 1.0) - modAmount * srcValue;
        else
          modLeft = value - modAmount * srcValue;

        modRight = modLeft + modAmount;
      }
    }
  }

  if(clipped)
  {
    modLeft = getValue().getScaleConverter()->getControlPositionRange().clip(modLeft);
    modRight = getValue().getScaleConverter()->getControlPositionRange().clip(modRight);
  }
  return std::make_pair(modLeft, modRight);
}

std::pair<Glib::ustring, Glib::ustring> ModulateableParameter::getModRangeAsDisplayValues() const
{
  auto range = getModulationRange(false);

  auto first = modulationValueToDisplayString(range.first);
  auto second = modulationValueToDisplayString(range.second);
  return std::make_pair(first, second);
}

Glib::ustring ModulateableParameter::modulationValueToDisplayString(tControlPositionValue v) const
{
  if(isBiPolar())
  {
    v = v * 2 - 1;
  }

  auto scaleConverter = getValue().getScaleConverter();
  auto clipped = scaleConverter->getControlPositionRange().clip(v);
  auto displayValue = scaleConverter->controlPositionToDisplay(clipped);
  auto ret = scaleConverter->getDimension().stringize(displayValue);

  if(differs(clipped, v))
    ret = "! " + ret;

  return ret;
}

bool ModulateableParameter::isChangedFromLoaded() const
{
  return isAnyModChanged() || Parameter::isChangedFromLoaded();
}

bool ModulateableParameter::isAnyModChanged() const
{
  return isModSourceChanged() || isModAmountChanged();
}

bool ModulateableParameter::isModAmountChanged() const
{
  if(getModulationSource() == MacroControls::NONE)
    return false;

  if(auto original = getOriginalParameter())
  {
    const int denominator = static_cast<const int>(getValue().getFineDenominator());
    const int roundedNow = static_cast<const int>(getModulationAmount() * denominator);
    const int roundedOG = static_cast<const int>(original->getRecallModulationAmount() * denominator);
    return roundedOG != roundedNow;
  }
  return false;
}

bool ModulateableParameter::isModSourceChanged() const
{
  if(auto original = getOriginalParameter())
  {
    return original->getRecallModSource() != getModulationSource();
  }
  return false;
}

bool ModulateableParameter::isMacroControlAssignedAndChanged() const
{
  if(getModulationSource() == MacroControls::NONE)
    return false;

  if(auto myCurrMC = getMacroControl())
    return myCurrMC->isChangedFromLoaded();

  return false;
}

MacroControlParameter *ModulateableParameter::getMacroControl() const
{
  auto src = getModulationSource();
  if(src != MacroControls::NONE)
  {
    auto myMCID = MacroControlsGroup::modSrcToParamId(src);
    return dynamic_cast<MacroControlParameter *>(
        Application::get().getPresetManager()->getEditBuffer()->findParameterByID(myMCID));
  }
  return nullptr;
}

void ModulateableParameter::undoableRecallMCPos()
{
  if(auto mc = getMacroControl())
  {
    mc->undoableRecallFromPreset();
    onChange(ChangeFlags::Generic);
  }
}

void ModulateableParameter::undoableRecallMCSource()
{
  if(!isModSourceChanged())
    return;
  auto &scope = Application::get().getPresetManager()->getUndoScope();
  auto original = getOriginalParameter();
  auto transactionScope = scope.startTransaction("Recall MC Pos");
  auto transaction = transactionScope->getTransaction();
  if(original)
  {
    setModulationSource(transaction, original->getRecallModSource());
  }
  onChange(ChangeFlags::Generic);
}

void ModulateableParameter::undoableRecallMCAmount()
{
  if(!isModAmountChanged())
    return;
  auto &scope = Application::get().getPresetManager()->getUndoScope();
  auto original = getOriginalParameter();
  auto transactionScope = scope.startTransaction("Recall MC Pos");
  auto transaction = transactionScope->getTransaction();
  if(original)
  {
    setModulationAmount(transaction, original->getRecallModulationAmount());
  }
  onChange(ChangeFlags::Generic);
}

void ModulateableParameter::copyFrom(UNDO::Transaction *transaction, const Parameter *other)
{
  Parameter::copyFrom(transaction, other);

  if(!isLocked())
  {
    if(auto otherMod = dynamic_cast<const ModulateableParameter *>(other))
    {
      setModulationSource(transaction, otherMod->getModulationSource());
      setModulationAmount(transaction, otherMod->getModulationAmount());
    }
  }
}

bool ModulateableParameter::isDefaultLoaded() const
{
  auto valSame = Parameter::isDefaultLoaded();
  auto modSrcSame = getModulationSource() == MacroControls::NONE;
  auto modAmtSame = getModulationAmount() == 0;
  return valSame && modSrcSame && modAmtSame;
}

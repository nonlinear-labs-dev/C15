#include <parameters/names/ParameterDB.h>
#include "Parameter.h"
#include "groups/ParameterGroup.h"
#include "libundo/undo/SwapData.h"
#include "xml/Writer.h"
#include "parameters/ValueScaling.h"
#include "scale-converters/ScaleConverter.h"
#include "proxies/lpc/MessageComposer.h"
#include "http/UpdateDocumentMaster.h"
#include "Application.h"
#include "proxies/lpc/LPCProxy.h"
#include "proxies/audio-engine/AudioEngineProxy.h"
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterInfoLayout.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/UnmodulatebaleParameterLayouts.h>
#include <presets/EditBuffer.h>
#include <presets/PresetManager.h>
#include <libundo/undo/Transaction.h>
#include <presets/PresetParameter.h>
#include <presets/Preset.h>
#include <device-settings/DebugLevel.h>
#include <nltools/Assert.h>
#include <parameters/messaging/ParameterMessageFactory.h>
#include <presets/recall/RecallParameter.h>
#include <xml/Attribute.h>
#include <http/UndoScope.h>
#include <proxies/hwui/HWUI.h>

static const auto c_invalidSnapshotValue = std::numeric_limits<tControlPositionValue>::max();

Parameter::Parameter(ParameterGroup *group, ParameterId id, const ScaleConverter *scaling, tControlPositionValue def,
                     tControlPositionValue coarseDenominator, tControlPositionValue fineDenominator)
    : UpdateDocumentContributor(group)
    , m_id(id)
    , m_value(this, scaling, def, coarseDenominator, fineDenominator)
    , m_lastSnapshotedValue(c_invalidSnapshotValue)
    , m_voiceGroup { group->getVoiceGroup() }
{
}

Parameter::~Parameter()
{
}

size_t Parameter::getHash() const
{
  size_t hash = 0;
  hash_combine(hash, m_value.getHash());
  return hash;
}

QuantizedValue &Parameter::getValue()
{
  return m_value;
}

const QuantizedValue &Parameter::getValue() const
{
  return m_value;
}

void Parameter::onValueChanged(Initiator initiator, tControlPositionValue oldValue, tControlPositionValue newValue)
{
  if(initiator == Initiator::EXPLICIT_LPC)
  {
    if(m_lastSnapshotedValue == c_invalidSnapshotValue)
    {
      m_lastSnapshotedValue = oldValue;
    }
  }
  else
  {
    m_lastSnapshotedValue = getControlPositionValue();
  }

  invalidate();
}

void Parameter::onValueFineQuantizedChanged(Initiator initiator, tControlPositionValue oldValue,
                                            tControlPositionValue newValue)
{
}

VoiceGroup Parameter::getVoiceGroup() const
{
  return m_voiceGroup;
}

tControlPositionValue Parameter::expropriateSnapshotValue()
{
  tControlPositionValue v = m_lastSnapshotedValue;
  m_lastSnapshotedValue = c_invalidSnapshotValue;
  return v == c_invalidSnapshotValue ? getControlPositionValue() : v;
}

void Parameter::setDefaultFromHwui()
{
  auto scope = getUndoScope().startContinuousTransaction(this, "Set '%0'", getGroupAndParameterName());
  setCPFromHwui(scope->getTransaction(), getDefaultValue());
}

void Parameter::setDefaultFromHwui(UNDO::Transaction *transaction)
{
  setCPFromHwui(transaction, getDefaultValue());
}

void Parameter::setDefaultFromWebUI(UNDO::Transaction *transaction)
{
  setCPFromWebUI(transaction, getDefaultValue());
}

void Parameter::stepCPFromHwui(UNDO::Transaction *transaction, int incs, ButtonModifiers modifiers)
{
  setCPFromHwui(transaction, getNextStepValue(incs, modifiers));
}

void Parameter::stepCPFromWebUI(UNDO::Transaction *transaction, Step step, ButtonModifiers modifiers)
{
  setCPFromWebUI(transaction, getNextStepValue(step == Parameter::Step::STEP_INC ? 1 : -1, modifiers));
}

void Parameter::setCPFromHwui(UNDO::Transaction *transaction, const tControlPositionValue &cpValue)
{
  setCpValue(transaction, Initiator::EXPLICIT_HWUI, cpValue, true);
}

void Parameter::setCPFromWebUI(UNDO::Transaction *transaction, const tControlPositionValue &cpValue)
{
  setCpValue(transaction, Initiator::EXPLICIT_WEBUI, cpValue, true);
}

void Parameter::loadFromPreset(UNDO::Transaction *transaction, const tControlPositionValue &value)
{
  auto q = getValue().getQuantizedValue(value, true);
  setIndirect(transaction, q);
  m_lastSnapshotedValue = q;
}

void Parameter::setIndirect(UNDO::Transaction *transaction, const tControlPositionValue &value)
{
  if(differs(value, m_value.getRawValue()))
  {
    auto swapData = UNDO::createSwapData(value);

    transaction->addSimpleCommand([=](UNDO::Command::State) mutable {
      tDisplayValue newVal = m_value.getRawValue();
      swapData->swapWith(newVal);
      m_value.setRawValue(Initiator::INDIRECT, newVal);
    });
  }
}

void Parameter::loadDefault(UNDO::Transaction *transaction)
{
  loadFromPreset(transaction, getDefaultValue());
}

bool Parameter::isDefaultLoaded() const
{
  return !isValueDifferentFrom(getDefaultValue());
}

void Parameter::reset(UNDO::Transaction *transaction, Initiator initiator)
{
  setCpValue(transaction, initiator, getDefaultValue(), true);
}

void Parameter::setCpValue(UNDO::Transaction *transaction, Initiator initiator, tControlPositionValue value,
                           bool doSendToLpc)
{
  if(m_value.differs(value))
  {
    if(initiator == Initiator::EXPLICIT_LPC)
    {
      m_value.setRawValue(Initiator::EXPLICIT_LPC, value);
    }
    else
    {
      auto swapData = UNDO::createSwapData(value);

      transaction->addSimpleCommand([=](UNDO::Command::State) mutable {
        tDisplayValue newVal = m_value.getRawValue();
        swapData->swapWith(newVal);

        m_value.setRawValue(initiator, newVal);

        if(doSendToLpc)
          sendToLpc();
      });
    }
  }
}

void Parameter::copyFrom(UNDO::Transaction *transaction, const PresetParameter *other)
{
  if(!isLocked())
  {
    loadFromPreset(transaction, other->getValue());
  }
}

void Parameter::copyTo(UNDO::Transaction *transaction, PresetParameter *other) const
{
  other->setValue(transaction, getValue().getRawValue());
}

void Parameter::undoableSetDefaultValue(UNDO::Transaction *transaction, const PresetParameter *value)
{
  tControlPositionValue v = value ? value->getValue() : m_value.getFactoryDefaultValue();
  undoableSetDefaultValue(transaction, v);
}

void Parameter::undoableSetDefaultValue(UNDO::Transaction *transaction, tControlPositionValue value)
{
  auto diff = value - m_value.getDefaultValue();

  if(std::abs(diff) > std::numeric_limits<tControlPositionValue>::epsilon())
  {
    auto swapData = UNDO::createSwapData(value);

    transaction->addSimpleCommand([=](UNDO::Command::State) mutable {
      tDisplayValue newVal = m_value.getDefaultValue();
      swapData->swapWith(newVal);
      m_value.setDefaultValue(newVal);
      invalidate();
    });
  }
}

void Parameter::sendToLpc() const
{
  sendParameterMessage();
}

tControlPositionValue Parameter::getNextStepValue(int incs, ButtonModifiers modifiers) const
{
  return m_value.getNextStepValue(incs, modifiers);
}

const RecallParameter *Parameter::getOriginalParameter() const
{
  auto eb = static_cast<EditBuffer *>(getParentGroup()->getParent());
  auto ret = eb->getRecallParameterSet().findParameterByID(getID());
  nltools_detailedAssertAlways(ret != nullptr, "originalParameter is null and should not be");
  return ret;
}

bool Parameter::isChangedFromLoaded() const
{
  return isValueChangedFromLoaded();
}

bool Parameter::isValueChangedFromLoaded() const
{
  const int denominator = static_cast<const int>(getValue().getFineDenominator());
  const int roundedNow = static_cast<const int>(std::round(getControlPositionValue() * denominator));
  const int roundedOG = static_cast<const int>(std::round(getOriginalParameter()->getRecallValue() * denominator));
  return roundedOG != roundedNow;
}

bool Parameter::isBiPolar() const
{
  return m_value.isBiPolar();
}

const ParameterGroup *Parameter::getParentGroup() const
{
  return static_cast<const ParameterGroup *>(getParent());
}

ParameterGroup *Parameter::getParentGroup()
{
  return static_cast<ParameterGroup *>(getParent());
}

ParameterId Parameter::getID() const
{
  return m_id;
}

tControlPositionValue Parameter::getDefaultValue() const
{
  return m_value.getDefaultValue();
}

tTcdValue Parameter::getTcdValue() const
{
  return m_value.getTcdValue();
}

tDisplayValue Parameter::getDisplayValue() const
{
  return m_value.getDisplayValue();
}

Glib::ustring Parameter::getDisplayString() const
{
  return m_value.getDisplayString();
}

Glib::ustring Parameter::getDisplayString(tControlPositionValue cp) const
{
  return m_value.getDisplayString(cp);
}

tControlPositionValue Parameter::getControlPositionValue() const
{
  return m_value.getQuantizedClipped();
}

sigc::connection Parameter::onParameterChanged(sigc::slot<void, const Parameter *> slot, bool doInitCall) const
{
  if(doInitCall)
    return m_signalParamChanged.connectAndInit(slot, this);

  return m_signalParamChanged.connect(slot);
}

void Parameter::invalidate()
{
  m_signalParamChanged.send(this);
  onChange();
}

Glib::ustring Parameter::getLongName() const
{
  return ParameterDB::get().getLongName(getID());
}

Glib::ustring Parameter::getShortName() const
{
  return ParameterDB::get().getShortName(getID());
}

Glib::ustring Parameter::getInfoText() const
{
  return ParameterDB::get().getDescription(getID());
}

Glib::ustring Parameter::getMiniParameterEditorName() const
{
  return getShortName();
}

Glib::ustring Parameter::getGroupAndParameterName() const
{
  return UNDO::StringTools::buildString(getParentGroup()->getShortName(), " - ", getLongName());
}

Glib::ustring Parameter::getGroupAndParameterNameWithVoiceGroup() const
{
  return UNDO::StringTools::buildString(getGroupAndParameterName(), "-", toString(getVoiceGroup()));
}

void Parameter::writeDocument(Writer &writer, tUpdateID knownRevision) const
{
  bool changed = knownRevision < getUpdateIDOfLastChange();

  writer.writeTag("parameter", Attribute("id", getID()), Attribute("changed", changed), Attribute("locked", isLocked()),
                  [&]() {
                    if(changed)
                    {
                      writeDocProperties(writer, knownRevision);
                    }
                  });
}

void Parameter::writeDocProperties(Writer &writer, tUpdateID knownRevision) const
{
  writer.writeTextElement("value", to_string(m_value.getRawValue()));
  writer.writeTextElement("default", to_string(m_value.getDefaultValue()));
  writer.writeTextElement("boolean", to_string(m_value.isBoolean()));

  if(shouldWriteDocProperties(knownRevision))
  {
    writer.writeTextElement("bipolar", to_string(m_value.isBiPolar()));
    writer.writeTextElement("scaling", m_value.getScaleConverter()->controlPositionToDisplayJS());
    writer.writeTextElement("long-name", getLongName());
    writer.writeTextElement("short-name", getShortName());
    writer.writeTextElement("info-text", getInfoText());
    writer.writeTextElement("coarse-denominator", to_string(m_value.getCoarseDenominator()));
    writer.writeTextElement("fine-denominator", to_string(m_value.getFineDenominator()));
  }
}

void Parameter::writeDiff(Writer &writer, Parameter *other) const
{
  if(getHash() != other->getHash())
  {
    writer.writeTag("parameter", Attribute("name", getLongName()), [&] { writeDifferences(writer, other); });
  }
}

void Parameter::writeDifferences(Writer &writer, Parameter *other) const
{
  auto myString = getDisplayString();
  auto otherString = other->getDisplayString();

  if(myString != otherString)
  {
    writer.writeTextElement("value", "", Attribute("a", myString), Attribute("b", otherString));
  }
}

bool Parameter::shouldWriteDocProperties(UpdateDocumentContributor::tUpdateID knownRevision) const
{
  return knownRevision == 0;
}

void Parameter::writeToLPC(MessageComposer &cmp) const
{
  gint16 v = getTcdValue();
  cmp << v;
}

void Parameter::undoableLoadValue(UNDO::Transaction *transaction, const Glib::ustring &value)
{
  auto tcdValue = std::stoi(value);
  auto cpValue = m_value.getScaleConverter()->tcdToControlPosition(tcdValue);
  loadFromPreset(transaction, cpValue);
}

void Parameter::undoableRandomize(UNDO::Transaction *transaction, Initiator initiator, double amount)
{
  auto rnd = g_random_double_range(0.0, 1.0);
  auto range = getValue().getScaleConverter()->getControlPositionRange();
  auto newPos
      = (getControlPositionValue() * (1.0 - amount)) + ((range.getMin() + rnd * range.getRangeWidth()) * amount);

  auto quantized = getValue().getQuantizedValue(newPos, true);
  setCpValue(transaction, initiator, quantized, false);
}

void Parameter::exportReaktorParameter(std::stringstream &target) const
{
  target << getTcdValue() << std::endl;
}

Layout *Parameter::createLayout(FocusAndMode focusAndMode) const
{
  switch(focusAndMode.mode)
  {
    case UIMode::Info:
      return new ParameterInfoLayout();

    case UIMode::Edit:
      return new UnmodulateableParameterEditLayout2();

    case UIMode::Select:
    default:
      return new UnmodulateableParameterSelectLayout2();
  }

  g_return_val_if_reached(nullptr);
}

void Parameter::onUnselected()
{
  onChange();
}

void Parameter::onSelected()
{
  onChange();
}

Parameter::VisualizationStyle Parameter::getVisualizationStyle() const
{
  switch(getID().getNumber())
  {
    case 135:
    case 155:
    case 113:
    case 136:
    case 243:
    case 246:
    case 244:
    case 245:
    case 348:
    case 350:
    case 352:
    case 354:
    case 362:
      return VisualizationStyle::Dot;
    case 396:  //Fade From II
      if(getID().getVoiceGroup() == VoiceGroup::II)
        return VisualizationStyle::BarFromRight;
      else  //Fade From I
        return VisualizationStyle::Bar;
    case 356:  //Split Point
      if(Application::get().getHWUI()->getCurrentVoiceGroup() == VoiceGroup::II)
        return VisualizationStyle::BarFromRight;
      else
        return VisualizationStyle::Bar;
  }
  return VisualizationStyle::Bar;
}

bool Parameter::lockingEnabled() const
{
  return true;
}

void Parameter::undoableLock(UNDO::Transaction *transaction)
{
  if(!isLocked())
  {
    auto swapData = UNDO::createSwapData(true);

    transaction->addSimpleCommand([=](auto) mutable {
      swapData->swapWith(m_isLocked);
      onChange(ChangeFlags::LockState);
    });
  }
}

void Parameter::undoableUnlock(UNDO::Transaction *transaction)
{
  if(isLocked())
  {
    auto swapData = UNDO::createSwapData(false);

    transaction->addSimpleCommand([=](auto) mutable {
      swapData->swapWith(m_isLocked);
      onChange(ChangeFlags::LockState);
    });
  }
}

bool Parameter::isLocked() const
{
  return m_isLocked;
}

void Parameter::check()
{
  auto cp = 0;

  auto scope = getUndoScope().startTransaction("Test parameter '%1'", getLongName());
  setCpValue(scope->getTransaction(), Initiator::EXPLICIT_OTHER, m_value.getLowerBorder(), false);

  // test inc and dec reaches min and max:

  for(int i = 0; i < m_value.getCoarseDenominator(); i++)
    stepCPFromHwui(scope->getTransaction(), 1, ButtonModifiers());

  cp = getControlPositionValue();
  g_assert(cp == m_value.getUpperBorder());

  for(int i = 0; i < m_value.getCoarseDenominator(); i++)
    stepCPFromHwui(scope->getTransaction(), -1, ButtonModifiers());

  cp = getControlPositionValue();
  g_assert(cp == m_value.getLowerBorder());

  for(int i = 0; i < m_value.getFineDenominator(); i++)
    stepCPFromHwui(scope->getTransaction(), 1, ButtonModifiers(ButtonModifier::FINE));

  cp = getControlPositionValue();
  g_assert(cp == m_value.getUpperBorder());

  for(int i = 0; i < m_value.getFineDenominator(); i++)
    stepCPFromHwui(scope->getTransaction(), -1, ButtonModifiers(ButtonModifier::FINE));

  cp = getControlPositionValue();
  g_assert(cp == m_value.getLowerBorder());
}

void Parameter::undoableRecallFromPreset()
{
  auto &scope = Application::get().getPresetManager()->getUndoScope();
  auto transactionScope = scope.startTransaction("Recall %0 value", getLongName());
  auto transaction = transactionScope->getTransaction();
  undoableRecallFromPreset(transaction);
}

void Parameter::undoableRecallFromPreset(UNDO::Transaction *transaction)
{
  auto original = getOriginalParameter();
  if(original)
    setCPFromHwui(transaction, original->getRecallValue());
  else
    setDefaultFromHwui(transaction);
}

void Parameter::copyFrom(UNDO::Transaction *transaction, const Parameter *other)
{
  if(!isLocked())
  {
    setCpValue(transaction, Initiator::INDIRECT, other->getControlPositionValue(), false);
  }
}

void Parameter::sendParameterMessage() const
{
  Application::get().getAudioEngineProxy()->createAndSendParameterMessage<Parameter>(this);
}

bool Parameter::isValueDifferentFrom(double d) const
{
  return m_value.differs(getValue().getQuantizedValue(d, true));
}

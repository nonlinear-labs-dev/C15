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
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterInfoLayout.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/UnmodulatebaleParameterLayouts.h>
#include <presets/EditBuffer.h>

static const auto c_invalidSnapshotValue = numeric_limits<tControlPositionValue>::max();

Parameter::Parameter(ParameterGroup *group, uint16_t id, const ScaleConverter *scaling, tControlPositionValue def,
                     tControlPositionValue coarseDenominator, tControlPositionValue fineDenominator) :
    UpdateDocumentContributor(group),
    m_id(id),
    m_value(this, scaling, def, coarseDenominator, fineDenominator),
    m_lastSnapshotedValue(c_invalidSnapshotValue)
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

void Parameter::setDefaultFromHwui(UNDO::Scope::tTransactionPtr transaction)
{
  setCPFromHwui(transaction, getDefaultValue());
}

void Parameter::setDefaultFromWebUI(UNDO::Scope::tTransactionPtr transaction)
{
  setCPFromWebUI(transaction, getDefaultValue());
}

void Parameter::stepCPFromHwui(UNDO::Scope::tTransactionPtr transaction, int incs, ButtonModifiers modifiers)
{
  setCPFromHwui(transaction, getNextStepValue(incs, modifiers));
}

void Parameter::stepCPFromWebUI(UNDO::Scope::tTransactionPtr transaction, Step step, ButtonModifiers modifiers)
{
  setCPFromWebUI(transaction, getNextStepValue(step == Parameter::Step::STEP_INC ? 1 : -1, modifiers));
}

void Parameter::setCPFromHwui(UNDO::Scope::tTransactionPtr transaction, const tControlPositionValue &cpValue)
{
  setCpValue(transaction, Initiator::EXPLICIT_HWUI, cpValue, true);
}

void Parameter::setCPFromWebUI(UNDO::Scope::tTransactionPtr transaction, const tControlPositionValue &cpValue)
{
  setCpValue(transaction, Initiator::EXPLICIT_WEBUI, cpValue, true);
}

void Parameter::loadFromPreset(UNDO::Scope::tTransactionPtr transaction, const tControlPositionValue &value)
{
  setIndirect(transaction, value);
  m_lastSnapshotedValue = value;
}

void Parameter::setIndirect(UNDO::Scope::tTransactionPtr transaction, const tControlPositionValue &value)
{
  auto swapData = UNDO::createSwapData(value);

  transaction->addSimpleCommand([ = ] (UNDO::Command::State) mutable
  {
    tDisplayValue newVal = m_value.getRawValue();
    swapData->swapWith (newVal);
    m_value.setRawValue (Initiator::INDIRECT, newVal);
  });
}

void Parameter::loadDefault(UNDO::Scope::tTransactionPtr transaction)
{
  loadFromPreset(transaction, getDefaultValue());
}

void Parameter::reset(UNDO::Scope::tTransactionPtr transaction, Initiator initiator)
{
  setCpValue(transaction, initiator, getDefaultValue(), true);
}

void Parameter::setCpValue(UNDO::Scope::tTransactionPtr transaction, Initiator initiator, tControlPositionValue value, bool doSendToLpc)
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

      transaction->addSimpleCommand([ = ] (UNDO::Command::State) mutable
      {
        tDisplayValue newVal = m_value.getRawValue();
        swapData->swapWith (newVal);

        m_value.setRawValue (initiator, newVal);

        if (doSendToLpc)
        sendToLpc ();
      });
    }
  }
}

void Parameter::copyFrom(UNDO::Scope::tTransactionPtr transaction, Parameter * other)
{
  if(!isLocked())
  {
    loadFromPreset(transaction, other->getControlPositionValue());
  }
}

void Parameter::undoableSetDefaultValue(UNDO::Scope::tTransactionPtr transaction, Parameter *value)
{
  if(value->getControlPositionValue() != m_value.getDefaultValue())
  {
    auto swapData = UNDO::createSwapData(value->getControlPositionValue());

    transaction->addSimpleCommand([ = ] (UNDO::Command::State) mutable
    {
      tDisplayValue newVal = m_value.getDefaultValue();
      swapData->swapWith (newVal);
      m_value.setDefaultValue(newVal);
      invalidate();
    });
  }
}

void Parameter::sendToLpc() const
{
  if(dynamic_cast<const EditBuffer*>(getParentGroup()->getParent()))
    Application::get().getLPCProxy()->sendParameter(this);
}

tControlPositionValue Parameter::getNextStepValue(int incs, ButtonModifiers modifiers) const
{
  return m_value.getNextStepValue(incs, modifiers);
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

gint32 Parameter::getID() const
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

tControlPositionValue Parameter::getControlPositionValue() const
{
  return m_value.getQuantizedClipped();
}

sigc::connection Parameter::onParameterChanged(slot<void, const Parameter *> slot, bool doInitCall)
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

Glib::ustring Parameter::getMiniParameterEditorName() const
{
  return getShortName();
}

Glib::ustring Parameter::getGroupAndParameterName() const
{
  return UNDO::StringTools::buildString(getParentGroup()->getShortName(), " - ", getLongName());
}

void Parameter::writeDocument(Writer &writer, tUpdateID knownRevision) const
{
  bool changed = knownRevision < getUpdateIDOfLastChange();

  writer.writeTag("parameter", Attribute("id", getID()), Attribute("changed", changed), Attribute("locked", isLocked()), [&]()
  {
    if (changed)
    {
      writeDocProperties (writer, knownRevision);
    }
  });
}

void Parameter::writeDiff(Writer &writer, Parameter *other) const
{
  if(getHash() != other->getHash())
  {
    writer.writeTag("parameter", Attribute("name", getLongName()), [&]
    {
      writeDifferences(writer, other);
    });
  }
}

void Parameter::writeDifferences(Writer& writer, Parameter* other) const
{
  auto myString = getDisplayString();
  auto otherString = other->getDisplayString();

  if(myString != otherString)
  {
    writer.writeTextElement("value", "", Attribute("a", myString), Attribute("b", otherString));
  }
}

void Parameter::writeDocProperties(Writer &writer, tUpdateID knownRevision) const
{
  writer.writeTextElement("value", to_string(m_value.getRawValue()));
  writer.writeTextElement("default", to_string(m_value.getDefaultValue()));

  if(shouldWriteDocProperties(knownRevision))
  {
    writer.writeTextElement("scaling", m_value.getScaleConverter()->controlPositionToDisplayJS());
    writer.writeTextElement("bipolar", to_string(m_value.isBiPolar()));
    writer.writeTextElement("long-name", getLongName());
    writer.writeTextElement("short-name", getShortName());
    writer.writeTextElement("coarse-denominator", to_string(m_value.getCoarseDenominator()));
    writer.writeTextElement("fine-denominator", to_string(m_value.getFineDenominator()));
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

void Parameter::undoableLoadValue(UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &value)
{
  auto tcdValue = stoi(value);
  auto cpValue = m_value.getScaleConverter()->tcdToControlPosition(tcdValue);
  loadFromPreset(transaction, cpValue);
}

void Parameter::undoableRandomize(UNDO::Scope::tTransactionPtr transaction, Initiator initiator, double amount)
{
  auto rnd = g_random_double_range(0.0, 1.0);
  auto range = getValue().getScaleConverter()->getControlPositionRange();
  auto newPos = (getControlPositionValue() * (1.0 - amount)) + ((range.getMin() + rnd * range.getRangeWidth()) * amount);
  setCpValue(transaction, initiator, newPos, false);
}

void Parameter::undoableSetType(UNDO::Scope::tTransactionPtr transaction, PresetType oldType, PresetType desiredType)
{
  m_value.undoableSetType(transaction, oldType, desiredType);
}

void Parameter::onPresetSentToLpc() const
{
}

void Parameter::exportReaktorParameter(stringstream &target) const
{
  target << getTcdValue() << endl;
}

DFBLayout *Parameter::createLayout(FocusAndMode focusAndMode) const
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
  switch(getID())
  {
    case 135:
    case 155:
    case 113:
    case 136:
      return VisualizationStyle::Dot;
  }
  return VisualizationStyle::Bar;
}

void Parameter::undoableLock(UNDO::Scope::tTransactionPtr transaction)
{
  if(!isLocked())
  {
    auto swapData = UNDO::createSwapData(true);

    transaction->addSimpleCommand([ = ] (UNDO::Command::State) mutable
    {
      swapData->swapWith (m_isLocked);
      onChange(ChangeFlags::LockState);
    });
  }
}

void Parameter::undoableUnlock(UNDO::Scope::tTransactionPtr transaction)
{
  if(isLocked())
  {
    auto swapData = UNDO::createSwapData(false);

    transaction->addSimpleCommand([ = ] (UNDO::Command::State) mutable
    {
      swapData->swapWith (m_isLocked);
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

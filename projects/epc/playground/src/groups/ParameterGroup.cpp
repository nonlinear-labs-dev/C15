#include "ParameterGroup.h"
#include "xml/Writer.h"
#include "parameters/Parameter.h"
#include "presets/ParameterGroupSet.h"
#include "presets/PresetParameterGroup.h"
#include <fstream>
#include <parameters/ModulateableParameter.h>
#include <parameters/MacroControlParameter.h>
#include <nltools/logging/Log.h>
#include <libundo/undo/Scope.h>
#include <xml/Attribute.h>
#include <nltools/logging/Log.h>

ParameterGroup::ParameterGroup(ParameterGroupSet *parent, GroupId id, const char *shortName, const char *longName,
                               const char *webUIName)
    : UpdateDocumentContributor(parent)
    , m_id(id)
    , m_shortName(shortName)
    , m_longName(longName)
    , m_webUIName(webUIName ? webUIName : m_longName)
{
}

ParameterGroup::~ParameterGroup()
{
  m_parameters.deleteItems();
}

GroupId ParameterGroup::getID() const
{
  return m_id;
}

size_t ParameterGroup::getHash() const
{
  size_t hash = 0;

  for(const auto p : m_parameters)
    hash_combine(hash, p->getHash());

  return hash;
}

Glib::ustring ParameterGroup::getShortName() const
{
  return m_shortName;
}

Glib::ustring ParameterGroup::getLongName() const
{
  return m_longName;
}

ParameterGroup::tParameterPtr ParameterGroup::getParameterByID(const ParameterId &id) const
{
  for(auto a : m_parameters)
    if(a->getID() == id)
      return a;

  nltools::throwException("Parameter not found!");
  return nullptr;
}

ParameterGroup::tParameterPtr ParameterGroup::findParameterByID(const ParameterId &id) const
{
  return getParameterByID(id);
}

ParameterGroup::tParameterPtr ParameterGroup::appendParameter(Parameter *p)
{
  m_parameters.append(p);
  return p;
}

sigc::connection ParameterGroup::onGroupChanged(const sigc::slot<void> &slot)
{
  return m_signalGroupChanged.connectAndInit(slot);
}

VoiceGroup ParameterGroup::getVoiceGroup() const
{
  return m_id.getVoiceGroup();
}

ParameterGroup::tUpdateID ParameterGroup::onChange(uint64_t flags)
{
  auto ret = super::onChange(flags);
  m_signalGroupChanged.deferedSend();
  return ret;
}

void ParameterGroup::copyFrom(UNDO::Transaction *transaction, const PresetParameterGroup *other)
{
  for(auto &myParameter : getParameters())
  {
    if(auto otherParameter = other->findParameterByID({ myParameter->getID().getNumber(), other->getVoiceGroup() }))
    {
      myParameter->copyFrom(transaction, otherParameter);
    }
    else
    {
      myParameter->loadDefault(transaction, Defaults::FactoryDefault);
    }
  }
}

void ParameterGroup::undoableSetDefaultValues(UNDO::Transaction *transaction, const PresetParameterGroup *other)
{
  for(auto &g : getParameters())
  {
    PresetParameter *p = other ? other->findParameterByID(g->getID()) : nullptr;
    g->undoableSetDefaultValue(transaction, p);
  }
}

void ParameterGroup::writeDocument(Writer &writer, tUpdateID knownRevision) const
{
  bool changed = knownRevision < getUpdateIDOfLastChange();

  writer.writeTag("parameter-group", Attribute("id", getID()), Attribute("short-name", getShortName()),
                  Attribute("long-name", m_webUIName), Attribute("changed", changed), [&]() {
                    if(changed)
                      for(const auto p : m_parameters)
                        p->writeDocument(writer, knownRevision);
                  });
}

void ParameterGroup::undoableReset(UNDO::Transaction *transaction, Initiator initiator)
{
  for(auto p : getParameters())
    p->reset(transaction, initiator);
}

void ParameterGroup::undoableRandomize(UNDO::Transaction *transaction, Initiator initiator, double amount)
{
  for(auto p : getParameters())
  {
    if(!p->isLocked() && !p->isDisabled())
    {
      p->undoableRandomize(transaction, initiator, amount);
    }
  }
}

void ParameterGroup::undoableLock(UNDO::Transaction *transaction)
{
  for(auto p : getParameters())
    p->undoableLock(transaction);
}

void ParameterGroup::undoableUnlock(UNDO::Transaction *transaction)
{
  for(auto p : getParameters())
    p->undoableUnlock(transaction);
}

void ParameterGroup::undoableToggleLock(UNDO::Transaction *transaction)
{
  if(areAllParametersLocked())
    undoableUnlock(transaction);
  else
    undoableLock(transaction);
}

bool ParameterGroup::isAnyParameterLocked() const
{
  for(auto p : getParameters())
    if(p->isLocked())
      return true;

  return false;
}

bool ParameterGroup::isAnyParameterChanged() const
{
  for(auto p : getParameters())
    if(p->isChangedFromLoaded())
      return true;

  return false;
}

bool ParameterGroup::areAllParametersLocked() const
{
  for(auto p : getParameters())
    if(!p->isLocked())
      return false;

  return true;
}

void ParameterGroup::copyFrom(UNDO::Transaction *transaction, const ParameterGroup *other)
{
  for(auto &myParameter : getParameters())
  {
    if(auto c = other->findParameterByID({ myParameter->getID().getNumber(), other->getVoiceGroup() }))
    {
      myParameter->copyFrom(transaction, c);
    }
    else
    {
      myParameter->loadDefault(transaction, Defaults::FactoryDefault);
    }
  }
}

void ParameterGroup::undoableLoadDefault(UNDO::Transaction *transaction, Defaults mode)
{
  for(auto &p : getParameters())
    p->loadDefault(transaction, mode);
}

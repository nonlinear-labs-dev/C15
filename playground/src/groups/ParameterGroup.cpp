#include "ParameterGroup.h"
#include "xml/Writer.h"
#include "parameters/Parameter.h"
#include "presets/ParameterGroupSet.h"
#include <fstream>

ParameterGroup::ParameterGroup(ParameterGroupSet *parent, const char *id, const char *shortName, const char *longName,
                               const char *webUIName) :
    UpdateDocumentContributor(parent),
    m_id(id),
    m_shortName(shortName),
    m_longName(longName),
    m_webUIName(webUIName ? : m_longName)
{
}

ParameterGroup::~ParameterGroup()
{
  m_parameters.deleteItems();
}

Glib::ustring ParameterGroup::getID() const
{
  return m_id;
}

size_t ParameterGroup::getHash() const
{
  size_t hash = 0;

  for(auto p : m_parameters)
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

size_t ParameterGroup::countParameters() const
{
  size_t i = 0;

  for(auto a : m_parameters)
    i++;

  return i;
}

ParameterGroup::tParameterPtr ParameterGroup::getParameterByID(gint32 id) const
{
  for(auto a : m_parameters)
  {
    if(a->getID() == id)
      return a;
  }

  return NULL;
}

map<int, pair<tDisplayValue, Glib::ustring>> &getDefaultValues()
{
  static map<int, pair<tDisplayValue, Glib::ustring>> m;

  if(m.empty())
  {
    std::ifstream in("/tmp/paramid-to-default.txt");

    char txt[1024];

    while(in.getline(txt, 1024))
    {
      Glib::ustring str(txt);
      auto cpos = str.find(',');

      if(cpos != Glib::ustring::npos)
      {
        Glib::ustring id = str.substr(0, cpos);
        Glib::ustring def = str.substr(cpos + 1);

        auto slashPos = def.find('/');

        pair<tDisplayValue, Glib::ustring> e;
        e.second = def;

        if(slashPos != Glib::ustring::npos)
        {
          Glib::ustring first = def.substr(0, slashPos);
          Glib::ustring second = def.substr(slashPos + 1);
          e.first = stod(first) / stod(second);
        }
        else
        {
          e.first = stod(def);
        }

        m[stoi(id)] = e;
      }
    }
  }

  return m;
}

ParameterGroup::tParameterPtr ParameterGroup::appendParameter(Parameter *p)
{
#if _TESTS
  auto &m = getDefaultValues ();

  if (!m.empty())
  {
    auto it = m.find (p->getID ());
    g_assert (it != m.end ());

    if (p->getDefaultValue () != it->second.first)
    {
      g_error ("Parameter %s (%d) in group %s has wrong default value. Current value is %f, should be %s\n",
          p->getLongName ().c_str (), p->getID (), getShortName ().c_str (), p->getDefaultValue (), it->second.second.c_str());
    }
  }
#endif

  m_parameters.append(p);
  return p;
}

sigc::connection ParameterGroup::onGroupChanged(slot<void> slot)
{
  return m_signalGroupChanged.connectAndInit(slot);
}

ParameterGroup::tUpdateID ParameterGroup::onChange(uint64_t flags)
{
  auto ret = super::onChange(flags);
  m_signalGroupChanged.send();
  return ret;
}

void ParameterGroup::copyFrom(UNDO::Scope::tTransactionPtr transaction, ParameterGroup* other)
{
  auto itThis = getParameters().begin();
  auto itOther = other->getParameters().begin();
  auto endThis = getParameters().end();
  auto endOther = other->getParameters().end();

  for(; itThis != endThis && itOther != endOther; (itThis++), (itOther++))
  {
    (*itThis)->copyFrom(transaction, *itOther);
  }
}

void ParameterGroup::undoableSetDefaultValues(UNDO::Scope::tTransactionPtr transaction, ParameterGroup *other)
{
  auto itThis = getParameters().begin();
  auto itOther = other->getParameters().begin();
  auto endThis = getParameters().end();
  auto endOther = other->getParameters().end();

  for(; itThis != endThis && itOther != endOther; (itThis++), (itOther++))
  {
    (*itThis)->undoableSetDefaultValue(transaction, *itOther);
  }
}

void ParameterGroup::writeDocument(Writer &writer, tUpdateID knownRevision) const
{
  bool changed = knownRevision < getUpdateIDOfLastChange();

  writer.writeTag("parameter-group", Attribute("id", getID()), Attribute("short-name", getShortName()), Attribute("long-name", m_webUIName),
      Attribute("changed", changed), [&]()
      {
        if (changed)
        for (const tParameterPtr p : m_parameters)
        p->writeDocument (writer, knownRevision);
      });
}

void ParameterGroup::writeDiff(Writer &writer, ParameterGroup *other) const
{
  if(getHash() != other->getHash())
  {
    writer.writeTag("group", Attribute("name", getLongName()), [&]
    {
      for(auto parameter : getParameters())
      {
        auto otherParameter = other->getParameterByID(parameter->getID());
        parameter->writeDiff(writer, otherParameter);
      }
    });
  }
}

void ParameterGroup::undoableClear(UNDO::Scope::tTransactionPtr transaction)
{
  for(auto p : getParameters())
  {
    if(!p->isLocked())
    {
      p->loadDefault(transaction);
    }
  }
}

void ParameterGroup::undoableReset(UNDO::Scope::tTransactionPtr transaction, Initiator initiator)
{
  for(auto p : getParameters())
    p->reset(transaction, initiator);
}

void ParameterGroup::undoableRandomize(UNDO::Scope::tTransactionPtr transaction, Initiator initiator, double amount)
{
  for(auto p : getParameters())
  {
    if(!p->isLocked())
    {
      p->undoableRandomize(transaction, initiator, amount);
    }
  }
}

void ParameterGroup::undoableSetType(UNDO::Scope::tTransactionPtr transaction, PresetType oldType, PresetType desiredType)
{
  for(auto p : getParameters())
    p->undoableSetType(transaction, oldType, desiredType);
}

void ParameterGroup::check()
{
  for(auto p : getParameters())
  {
    p->check();
    getUndoScope().reset();
  }
}

void ParameterGroup::undoableLock(UNDO::Scope::tTransactionPtr transaction)
{
  for(auto p : getParameters())
    p->undoableLock(transaction);
}

void ParameterGroup::undoableUnlock(UNDO::Scope::tTransactionPtr transaction)
{
  for(auto p : getParameters())
    p->undoableUnlock(transaction);
}

void ParameterGroup::undoableToggleLock(UNDO::Scope::tTransactionPtr transaction)
{
  if(areAllParametersLocked())
    undoableUnlock(transaction);
  else
    undoableLock(transaction);
}

bool ParameterGroup::areAllParametersLocked() const
{
  for(auto p : getParameters())
    if(!p->isLocked())
      return false;

  return true;
}


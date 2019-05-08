#include "ParameterGroupSet.h"

#include "../groups/GapFilterGroup.h"
#include "parameters/Parameter.h"

#include "groups/EnvelopeAGroup.h"
#include "groups/EnvelopeBGroup.h"
#include "groups/EnvelopeCGroup.h"
#include "groups/OscillatorAGroup.h"
#include "groups/ShaperAGroup.h"
#include "groups/OscillatorBGroup.h"
#include "groups/ShaperBGroup.h"
#include "groups/FBMixerGroup.h"
#include "groups/CombFilterGroup.h"
#include "groups/SVFilterGroup.h"
#include "groups/OutputMixerGroup.h"
#include "groups/CabinetGroup.h"
#include "groups/FlangerGroup.h"
#include "groups/EchoGroup.h"
#include "groups/ReverbGroup.h"
#include "groups/MacroControlsGroup.h"
#include "groups/MasterGroup.h"
#include "groups/UnisonGroup.h"
#include "groups/HardwareSourcesGroup.h"
#include <groups/MacroControlMappingGroup.h>
#include <groups/ScaleGroup.h>

#include "xml/Writer.h"

ParameterGroupSet::ParameterGroupSet(UpdateDocumentContributor *parent)
    : UpdateDocumentContributor(parent)
{
}

ParameterGroupSet::~ParameterGroupSet()
{
  m_parameterGroups.deleteItems();
}

void ParameterGroupSet::init()
{
  appendParameterGroup(new EnvelopeAGroup(this));
  appendParameterGroup(new EnvelopeBGroup(this));
  appendParameterGroup(new EnvelopeCGroup(this));
  appendParameterGroup(new OscillatorAGroup(this));
  appendParameterGroup(new ShaperAGroup(this));
  appendParameterGroup(new OscillatorBGroup(this));
  appendParameterGroup(new ShaperBGroup(this));
  appendParameterGroup(new FBMixerGroup(this));
  appendParameterGroup(new CombFilterGroup(this));
  appendParameterGroup(new SVFilterGroup(this));
  appendParameterGroup(new OutputMixerGroup(this));
  appendParameterGroup(new FlangerGroup(this));
  appendParameterGroup(new CabinetGroup(this));
  appendParameterGroup(new GapFilterGroup(this));
  appendParameterGroup(new EchoGroup(this));
  appendParameterGroup(new ReverbGroup(this));
  appendParameterGroup(new MasterGroup(this));
  appendParameterGroup(new UnisonGroup(this));

  auto macroControls = appendParameterGroup(new MacroControlsGroup(this));
  auto hwSources = appendParameterGroup(new HardwareSourcesGroup(this));
  appendParameterGroup(new MacroControlMappingGroup(this, hwSources, macroControls));

  appendParameterGroup(new ScaleGroup(this));

  m_idToParameterMap = getParametersSortedById();
}

ParameterGroupSet::tParameterGroupPtr ParameterGroupSet::getParameterGroupByID(const Glib::ustring &id) const
{
  for(auto a : m_parameterGroups)
  {
    if(a->getID() == id)
      return a;
  }

  return NULL;
}

ParameterGroupSet::tParameterGroupPtr ParameterGroupSet::appendParameterGroup(ParameterGroup *p)
{
  p->init();
  g_assert(getParameterGroupByID(p->getID()) == NULL);
  auto wrapped = tParameterGroupPtr(p);
  m_parameterGroups.append(wrapped);
  return wrapped;
}

void ParameterGroupSet::copyFrom(UNDO::Scope::tTransactionPtr transaction, ParameterGroupSet *other)
{
  auto itThis = getParameterGroups().begin();
  auto itOther = other->getParameterGroups().begin();
  auto endThis = getParameterGroups().end();
  auto endOther = other->getParameterGroups().end();

  for(; itThis != endThis && itOther != endOther; (itThis++), (itOther++))
  {
    (*itThis)->copyFrom(transaction, *itOther);
  }
}

Parameter *ParameterGroupSet::findParameterByID(size_t id) const
{
  try
  {
    return m_idToParameterMap.at(id);
  }
  catch(...)
  {
    return nullptr;
  }
}

size_t ParameterGroupSet::countParameters() const
{
  size_t count = 0;

  for(auto group : m_parameterGroups)
    count += group->countParameters();

  return count;
}

map<int, Parameter *> ParameterGroupSet::getParametersSortedById() const
{
  map<int, Parameter *> sorted;

  for(auto group : getParameterGroups())
    for(auto param : group->getParameters())
      sorted[param->getID()] = param;

  return sorted;
}

void ParameterGroupSet::writeDiff(Writer &writer, ParameterGroupSet *other) const
{
  writer.writeTag("groups", [&] {
    for(auto group : getParameterGroups())
    {
      auto otherGroup = other->getParameterGroupByID(group->getID());
      group->writeDiff(writer, otherGroup);
    }
  });
}

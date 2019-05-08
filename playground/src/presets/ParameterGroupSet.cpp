#include "ParameterGroupSet.h"
#include <presets/Preset.h>

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
    : super(parent)
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

ParameterGroupSet::~ParameterGroupSet()
{
  m_parameterGroups.deleteItems();
}

ParameterGroupSet::tParameterGroupPtr ParameterGroupSet::getParameterGroupByID(const Glib::ustring &id) const
{
  for(auto a : m_parameterGroups)
    if(a->getID() == id)
      return a;

  return nullptr;
}

ParameterGroupSet::tParameterGroupPtr ParameterGroupSet::appendParameterGroup(ParameterGroup *p)
{
  p->init();
  g_assert(getParameterGroupByID(p->getID()) == nullptr);
  auto wrapped = tParameterGroupPtr(p);
  m_parameterGroups.append(wrapped);
  return wrapped;
}

void ParameterGroupSet::copyFrom(UNDO::Transaction *transaction, const Preset *other)
{
  super::copyFrom(transaction, other);

  for(auto &g : getParameterGroups())
  {
    if(auto c = other->findParameterGroup(g->getID()))
    {
      g->copyFrom(transaction, c);
    }
  }
}

Parameter *ParameterGroupSet::findParameterByID(int id) const
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

std::map<int, Parameter *> ParameterGroupSet::getParametersSortedById() const
{
  std::map<int, Parameter *> sorted;

  for(auto group : getParameterGroups())
    for(auto param : group->getParameters())
      sorted[param->getID()] = param;

  return sorted;
}

void ParameterGroupSet::writeDocument(Writer &writer, UpdateDocumentContributor::tUpdateID knownRevision) const
{
  super::writeDocument(writer, knownRevision);

  for(const tParameterGroupPtr p : getParameterGroups())
    p->writeDocument(writer, knownRevision);
}

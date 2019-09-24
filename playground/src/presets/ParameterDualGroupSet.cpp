#include "ParameterDualGroupSet.h"
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

#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>

ParameterDualGroupSet::ParameterDualGroupSet(UpdateDocumentContributor *parent)
    : super(parent)
{
  for(auto vg : { VoiceGroup::I, VoiceGroup::II })
  {
    appendParameterGroup(new EnvelopeAGroup(this), vg);
    appendParameterGroup(new EnvelopeBGroup(this), vg);
    appendParameterGroup(new EnvelopeCGroup(this), vg);
    appendParameterGroup(new OscillatorAGroup(this), vg);
    appendParameterGroup(new ShaperAGroup(this), vg);
    appendParameterGroup(new OscillatorBGroup(this), vg);
    appendParameterGroup(new ShaperBGroup(this), vg);
    appendParameterGroup(new FBMixerGroup(this), vg);
    appendParameterGroup(new CombFilterGroup(this), vg);
    appendParameterGroup(new SVFilterGroup(this), vg);
    appendParameterGroup(new OutputMixerGroup(this), vg);
    appendParameterGroup(new FlangerGroup(this), vg);
    appendParameterGroup(new CabinetGroup(this), vg);
    appendParameterGroup(new GapFilterGroup(this), vg);
    appendParameterGroup(new EchoGroup(this), vg);
    appendParameterGroup(new ReverbGroup(this), vg);
    appendParameterGroup(new MasterGroup(this), vg);
    appendParameterGroup(new UnisonGroup(this), vg);

    auto macroControls = appendParameterGroup(new MacroControlsGroup(this), vg);
    auto hwSources = appendParameterGroup(new HardwareSourcesGroup(this), vg);
    appendParameterGroup(new MacroControlMappingGroup(this, hwSources, macroControls), vg);
    appendParameterGroup(new ScaleGroup(this), vg);

    m_idToParameterMap[static_cast<int>(vg)] = getParametersSortedById(vg);
  }
}

ParameterDualGroupSet::~ParameterDualGroupSet()
{
  for(auto &i : m_parameterGroups)
    i.deleteItems();
}

ParameterDualGroupSet::tParameterGroupPtr ParameterDualGroupSet::getParameterGroupByID(const Glib::ustring &id,
                                                                                       VoiceGroup vg) const
{
  for(auto a : m_parameterGroups[static_cast<int>(vg)])
    if(a->getID() == id)
      return a;

  return nullptr;
}

ParameterDualGroupSet::tParameterGroupPtr ParameterDualGroupSet::appendParameterGroup(ParameterGroup *p, VoiceGroup v)
{
  p->init();
  g_assert(getParameterGroupByID(p->getID(), v) == nullptr);
  auto wrapped = tParameterGroupPtr(p);
  m_parameterGroups[static_cast<int>(v)].append(wrapped);
  return wrapped;
}

void ParameterDualGroupSet::copyFrom(UNDO::Transaction *transaction, const Preset *other)
{
  copyFrom(transaction, other, m_selectedVoiceGroup);
}

void ParameterDualGroupSet::copyFrom(UNDO::Transaction *transaction, const Preset *other, VoiceGroup target)
{
  super::copyFrom(transaction, other);

  for(auto &g : getParameterGroups(target))
  {
    if(auto c = other->findParameterGroup(g->getID()))
    {
      g->copyFrom(transaction, c);
    }
  }
}

Parameter *ParameterDualGroupSet::findParameterByID(int id, VoiceGroup vg) const
{
  try
  {
    return m_idToParameterMap[static_cast<int>(vg)].at(id);
  }
  catch(...)
  {
    return nullptr;
  }
}

size_t ParameterDualGroupSet::countParameters() const
{
  size_t count = 0;

  for(auto group : m_parameterGroups[0])
    count += group->countParameters();

  return count;
}

std::map<int, Parameter *> ParameterDualGroupSet::getParametersSortedById() const
{
  return getParametersSortedById(m_selectedVoiceGroup);
}

std::map<int, Parameter *> ParameterDualGroupSet::getParametersSortedById(VoiceGroup vg) const
{
  std::map<int, Parameter *> sorted;

  for(auto group : getParameterGroups(vg))
    for(auto param : group->getParameters())
      sorted[param->getID()] = param;

  return sorted;
}

void ParameterDualGroupSet::writeDocument(Writer &writer, UpdateDocumentContributor::tUpdateID knownRevision) const
{
  super::writeDocument(writer, knownRevision);

  for(tParameterGroupPtr p : getParameterGroups())
    p->writeDocument(writer, knownRevision);
}

ParameterDualGroupSet::tParameterGroupPtr ParameterDualGroupSet::getParameterGroupByID(const Glib::ustring &id) const
{
  return getParameterGroupByID(id, m_selectedVoiceGroup);
}

Parameter *ParameterDualGroupSet::findParameterByID(int id) const
{
  return findParameterByID(id, m_selectedVoiceGroup);
}

const IntrusiveList<ParameterDualGroupSet::tParameterGroupPtr> &
    ParameterDualGroupSet::getParameterGroups(VoiceGroup vg) const
{
  return m_parameterGroups[static_cast<int>(vg)];
}

const IntrusiveList<ParameterDualGroupSet::tParameterGroupPtr> &ParameterDualGroupSet::getParameterGroups() const
{
  return getParameterGroups(m_selectedVoiceGroup);
}

void ParameterDualGroupSet::selectVoiceGroup(VoiceGroup group)
{
  if(std::exchange(m_selectedVoiceGroup, group) != group)
  {
    onChange();
  }
}

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
#include <groups/MonoGroup.h>
#include <groups/GlobalParameterGroups.h>
#include <groups/VoiceGroupMasterGroup.h>

ParameterDualGroupSet::ParameterDualGroupSet(UpdateDocumentContributor *parent)
    : super(parent)
{
  auto hwSources = appendParameterGroup(new HardwareSourcesGroup(this));
  auto macroControls = appendParameterGroup(new MacroControlsGroup(this));
  appendParameterGroup(new MacroControlMappingGroup(this, hwSources, macroControls));

  for(auto vg : { VoiceGroup::I, VoiceGroup::II })
  {
    appendParameterGroup(new EnvelopeAGroup(this, vg));
    appendParameterGroup(new EnvelopeBGroup(this, vg));
    appendParameterGroup(new EnvelopeCGroup(this, vg));
    appendParameterGroup(new OscillatorAGroup(this, vg));
    appendParameterGroup(new ShaperAGroup(this, vg));
    appendParameterGroup(new OscillatorBGroup(this, vg));
    appendParameterGroup(new ShaperBGroup(this, vg));
    appendParameterGroup(new FBMixerGroup(this, vg));
    appendParameterGroup(new CombFilterGroup(this, vg));
    appendParameterGroup(new SVFilterGroup(this, vg));
    appendParameterGroup(new OutputMixerGroup(this, vg));
    appendParameterGroup(new FlangerGroup(this, vg));
    appendParameterGroup(new CabinetGroup(this, vg));
    appendParameterGroup(new GapFilterGroup(this, vg));
    appendParameterGroup(new EchoGroup(this, vg));
    appendParameterGroup(new ReverbGroup(this, vg));
    appendParameterGroup(new UnisonGroup(this, vg));

    appendParameterGroup(new MonoGroup(this, vg));
    appendParameterGroup(new VoiceGroupMasterGroup(this, vg));

    m_idToParameterMap[static_cast<size_t>(vg)] = getParametersSortedByNumber(vg);
  }

  appendParameterGroup(new GlobalParameterGroups(this));
  appendParameterGroup(new MasterGroup(this));
  appendParameterGroup(new ScaleGroup(this));

  m_idToParameterMap[static_cast<size_t>(VoiceGroup::Global)] = getParametersSortedByNumber(VoiceGroup::Global);
}

ParameterDualGroupSet::~ParameterDualGroupSet()
{
  for(auto &i : m_parameterGroups)
    i.deleteItems();
}

ParameterDualGroupSet::tParameterGroupPtr ParameterDualGroupSet::getParameterGroupByID(const GroupId &id) const
{
  for(auto a : m_parameterGroups[static_cast<size_t>(id.getVoiceGroup())])
    if(a->getID().getName() == id.getName())
      return a;

  return nullptr;
}

ParameterDualGroupSet::tParameterGroupPtr ParameterDualGroupSet::appendParameterGroup(ParameterGroup *p)
{
  p->init();
  g_assert(getParameterGroupByID(p->getID()) == nullptr);
  auto wrapped = tParameterGroupPtr(p);
  m_parameterGroups[static_cast<size_t>(p->getID().getVoiceGroup())].append(wrapped);
  return wrapped;
}

void ParameterDualGroupSet::copyFrom(UNDO::Transaction *transaction, const Preset *other)
{
  super::copyFrom(transaction, other);

  for(auto vg : { VoiceGroup::Global, VoiceGroup::I, VoiceGroup::II })
    for(auto &g : getParameterGroups(vg))
      if(auto c = other->findParameterGroup(g->getID()))
        g->copyFrom(transaction, c);
      else
        g->forEachParameter([&](Parameter *p) { p->setDefaultFromHwui(transaction); });
}

Parameter *ParameterDualGroupSet::findParameterByID(const ParameterId &id) const
{
  try
  {
    return m_idToParameterMap.at(static_cast<size_t>(id.getVoiceGroup())).at(id.getNumber());
  }
  catch(...)
  {
    return nullptr;
  }
}

void ParameterDualGroupSet::forEachParameter(VoiceGroup vg, const std::function<void(Parameter *)> &cb)
{
  for(auto g : getParameterGroups(vg))
    for(auto p : g->getParameters())
      cb(p);
}

void ParameterDualGroupSet::forEachParameter(const std::function<void(Parameter *)> &cb)
{
  for(auto vg : { VoiceGroup::I, VoiceGroup::II, VoiceGroup::Global })
    for(auto g : getParameterGroups(vg))
      for(auto p : g->getParameters())
        cb(p);
}

void ParameterDualGroupSet::forEachParameter(const std::function<void(const Parameter *)> &cb) const
{
  for(auto vg : { VoiceGroup::I, VoiceGroup::II, VoiceGroup::Global })
    for(auto g : getParameterGroups(vg))
      for(auto p : g->getParameters())
        cb(p);
}

std::map<int, Parameter *> ParameterDualGroupSet::getParametersSortedByNumber(VoiceGroup vg) const
{
  std::map<int, Parameter *> sorted;

  for(auto group : getParameterGroups(vg))
    for(auto param : group->getParameters())
      sorted[param->getID().getNumber()] = param;

  return sorted;
}

void ParameterDualGroupSet::writeDocument(Writer &writer, UpdateDocumentContributor::tUpdateID knownRevision) const
{
  super::writeDocument(writer, knownRevision);

  writer.writeTag("global-parameters", [&] {
    for(tParameterGroupPtr p : getParameterGroups(VoiceGroup::Global))
      p->writeDocument(writer, knownRevision);
  });

  writer.writeTag("voice-group-I-parameters", [&] {
    for(tParameterGroupPtr p : getParameterGroups(VoiceGroup::I))
      p->writeDocument(writer, knownRevision);
  });

  writer.writeTag("voice-group-II-parameters", [&] {
    for(tParameterGroupPtr p : getParameterGroups(VoiceGroup::II))
      p->writeDocument(writer, knownRevision);
  });
}

const IntrusiveList<ParameterDualGroupSet::tParameterGroupPtr> &
    ParameterDualGroupSet::getParameterGroups(VoiceGroup vg) const
{
  return m_parameterGroups.at(static_cast<size_t>(vg));
}

void ParameterDualGroupSet::copyFrom(UNDO::Transaction *transaction, const Preset *preset, VoiceGroup from,
                                     VoiceGroup to)
{
  for(auto myGroup : getParameterGroups(to))
  {
    if(auto other = preset->findParameterGroup({ myGroup->getID().getName(), from }))
    {
      myGroup->copyFrom(transaction, other);
    }
  }
}

void ParameterDualGroupSet::copyVoiceGroup(UNDO::Transaction *transaction, VoiceGroup from, VoiceGroup to)
{
  for(auto &group : getParameterGroups(to))
    group->copyFrom(transaction, getParameterGroupByID({ group->getID().getName(), from }));
}

void ParameterDualGroupSet::loadIntoVoiceGroup(UNDO::Transaction *transaction, Preset *p, VoiceGroup target)
{
  nltools_assertOnDevPC(p->getType() == SoundType::Single);

  for(auto &g : getParameterGroups(target))
    if(auto c = p->findParameterGroup({ g->getID().getName(), VoiceGroup::I }))
      g->copyFrom(transaction, c);
}

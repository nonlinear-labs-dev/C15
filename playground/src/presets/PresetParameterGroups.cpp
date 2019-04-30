#include <Application.h>
#include "EditBuffer.h"
#include "PresetParameterGroups.h"
#include "PresetManager.h"
#include "Preset.h"

PresetParameterGroups::PresetParameterGroups(UpdateDocumentContributor *parent)
    : AttributesOwner(parent)
{
}

PresetParameterGroups::PresetParameterGroups(UpdateDocumentContributor *parent, const Preset &other)
    : AttributesOwner(parent, &other)
{
  init(&other);
}

PresetParameterGroups::PresetParameterGroups(UpdateDocumentContributor *parent, const EditBuffer &editbuffer)
    : AttributesOwner(parent, &editbuffer)
{
  for(auto &g : editbuffer.getParameterGroups())
    m_parameterGroups[g->getID()] = std::make_unique<PresetParameterGroup>(*g);
}

void PresetParameterGroups::copyFromParameterGroupSet(UNDO::Transaction *transaction, const ParameterGroupSet *other)
{
  for(auto &g : other->getParameterGroups())
    m_parameterGroups[g->getID()]->copyFrom(transaction, g);

  AttributesOwner::copyFrom(transaction, other);
}

void PresetParameterGroups::writeDocument(Writer &writer, tUpdateID knownRevision) const
{
  for(auto &pair : m_parameterGroups)
    pair.second->writeDocument(writer);
  AttributesOwner::writeDocument(writer, knownRevision);
}

void PresetParameterGroups::init(const ParameterGroupSet *other)
{
  for(auto &g : other->getParameterGroups())
    m_parameterGroups[g->getID()] = std::make_unique<PresetParameterGroup>(*g);
}

void PresetParameterGroups::init(const Preset *preset)
{
  std::for_each(preset->m_parameterGroups.begin(), preset->m_parameterGroups.end(), [this](auto &a) {
    m_parameterGroups[a.first] = std::make_unique<PresetParameterGroup>(*a.second.get());
  });
}

#include <Application.h>
#include <testing/TestRootDocument.h>
#include "EditBuffer.h"
#include "PresetDualParameterGroups.h"
#include "PresetManager.h"
#include "Preset.h"
#include <groups/ParameterGroup.h>
#include <presets/PresetParameterGroup.h>

PresetDualParameterGroups::PresetDualParameterGroups(UpdateDocumentContributor *parent)
    : AttributesOwner(parent)
{
  m_type = SoundType::Single;
  initEmpty();
}

PresetDualParameterGroups::PresetDualParameterGroups(UpdateDocumentContributor *parent, const Preset &other)
    : AttributesOwner(parent, &other)
{
  init(&other);
}

PresetDualParameterGroups::PresetDualParameterGroups(UpdateDocumentContributor *parent, const EditBuffer &editbuffer)
    : AttributesOwner(parent, &editbuffer)
{
  m_type = editbuffer.getType();

  for(auto vg : { VoiceGroup::Global, VoiceGroup::I, VoiceGroup::II })
    for(auto &g : editbuffer.getParameterGroups(vg))
      m_parameterGroups[static_cast<size_t>(vg)][g->getID()] = std::make_unique<PresetParameterGroup>(*g);
}

PresetDualParameterGroups::~PresetDualParameterGroups() = default;

void PresetDualParameterGroups::writeDocument(Writer &writer, tUpdateID knownRevision) const
{
  for(auto vg : { VoiceGroup::Global, VoiceGroup::I, VoiceGroup::II })
    for(auto &g : m_parameterGroups[static_cast<size_t>(vg)])
      g.second->writeDocument(writer);

  AttributesOwner::writeDocument(writer, knownRevision);
}

void PresetDualParameterGroups::copyFrom(UNDO::Transaction *transaction, const AttributesOwner *other)
{
  AttributesOwner::copyFrom(transaction, other);

  if(auto preset = dynamic_cast<const PresetDualParameterGroups *>(other))
  {
    for(auto &vg : { VoiceGroup::Global, VoiceGroup::I, VoiceGroup::II })
    {
      auto index = static_cast<size_t>(vg);
      for(auto &group : preset->m_parameterGroups[index])
      {
        auto groupId = group.first;
        auto &myGroup = m_parameterGroups[index][groupId];
        myGroup->copyFrom(transaction, group.second.get());
      }
    }
  }
}

void PresetDualParameterGroups::init(const Preset *preset)
{
  m_type = preset->getType();

  for(auto vg : { VoiceGroup::Global, VoiceGroup::I, VoiceGroup::II })
    for(auto &group : preset->m_parameterGroups[static_cast<size_t>(vg)])
      m_parameterGroups[static_cast<size_t>(vg)][group.first] = std::make_unique<PresetParameterGroup>(*group.second);
}

void PresetDualParameterGroups::initEmpty()
{
  static ParameterGroupSet sDataScheme(nullptr);

  for(auto vg : { VoiceGroup::Global, VoiceGroup::I, VoiceGroup::II })
    for(auto &group: sDataScheme.getParameterGroups(vg))
      m_parameterGroups[static_cast<size_t>(vg)][group->getID()] = std::make_unique<PresetParameterGroup>(*group);
}

PresetDualParameterGroups::GroupsMap &PresetDualParameterGroups::getGroups(VoiceGroup vg)
{
  return m_parameterGroups[static_cast<size_t>(vg)];
}
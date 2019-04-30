#include "RecallParameterGroups.h"
#include "presets/Preset.h"
#include "presets/EditBuffer.h"
#include "libundo/undo/Transaction.h"
#include "xml/Writer.h"

RecallParameterGroups::RecallParameterGroups(EditBuffer *editBuffer)
    : PresetParameterGroups(editBuffer, *editBuffer)
    , m_origin{ "EditBuffer" }
{
  for(auto &g : editBuffer->getParameterGroups())
    m_parameterGroups[g->getID()] = std::make_unique<PresetParameterGroup>(*g);
}

PresetParameter *RecallParameterGroups::findParameterByID(int id)
{
  for(auto &pair : m_parameterGroups)
  {
    if(auto param = pair.second->findParameterByID(id))
      return param;
  }
  return nullptr;
}

const PresetParameter *RecallParameterGroups::findParameterByID(int id) const
{
  return const_cast<RecallParameterGroups *>(this)->findParameterByID(id);
}

void RecallParameterGroups::copyFromEditBuffer(UNDO::Transaction *transaction, const EditBuffer *other)
{
  transaction->addUndoSwap(m_origin, Glib::ustring("EditBuffer"));
  PresetParameterGroups::copyFromParameterGroupSet(transaction, other);
}

void RecallParameterGroups::copyParamSet(UNDO::Transaction *transaction, const Preset *other)
{
  for(auto &pair : other->m_parameterGroups)
  {
    auto &othergroup = pair.second;
    m_parameterGroups.at(pair.first)->copyFrom(transaction, othergroup.get());
  }

  transaction->addUndoSwap(this, m_origin, Glib::ustring("Preset"));
}

void RecallParameterGroups::onPresetDeleted(UNDO::Transaction *transaction)
{
  transaction->addUndoSwap(this, m_origin, Glib::ustring("EditBuffer"));
}

void RecallParameterGroups::writeDocument(Writer &writer, UpdateDocumentContributor::tUpdateID knownRevision) const
{
  auto changed = getUpdateIDOfLastChange() > knownRevision;
  writer.writeTag("recall-data", Attribute{ "changed", changed }, [this, &writer, changed] {
    if(changed)
    {
      for(auto &pair : m_parameterGroups)
        pair.second->writeDocument(writer);
    }
  });

  AttributesOwner::writeDocument(writer, knownRevision);
}

UpdateDocumentContributor::tUpdateID RecallParameterGroups::onChange(uint64_t flags)
{
  m_signalRecallValues.send();
  return UpdateDocumentContributor::onChange(flags);
}

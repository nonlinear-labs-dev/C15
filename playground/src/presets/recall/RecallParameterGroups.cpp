#include <nltools/logging/Log.h>
#include "RecallParameterGroups.h"
#include "presets/Preset.h"
#include "presets/EditBuffer.h"
#include "libundo/undo/Transaction.h"
#include "xml/Writer.h"
#include "RecallParameter.h"
#include "presets/PresetParameter.h"

RecallParameterGroups::RecallParameterGroups(EditBuffer *editBuffer)
    : UpdateDocumentContributor(editBuffer)
    , m_editBuffer{ editBuffer }
{
  for(auto vg : { VoiceGroup::Global, VoiceGroup::I, VoiceGroup::II })
  {
    for(auto &g : m_editBuffer->getParameterGroups(vg))
    {
      for(auto &parameter : g->getParameters())
      {
        m_parameters[parameter->getID()] = std::make_unique<RecallParameter>(this, parameter->getID());
      }
    }
  }
}

RecallParameterGroups::tParameterMap &RecallParameterGroups::getParameters()
{
  return m_parameters;
}

const RecallParameterGroups::tParameterMap &RecallParameterGroups::getParameters() const
{
  return m_parameters;
}

RecallParameter *RecallParameterGroups::findParameterByID(const ParameterId &id) const
{
  try
  {
    return m_parameters.at(id).get();
  }
  catch(...)
  {
    nltools::Log::error("Could not find Recall Parameter with id", id.toString());
    return nullptr;
  }
}

void RecallParameterGroups::copyFromEditBuffer(UNDO::Transaction *transaction, const EditBuffer *other)
{
  for(auto vg : { VoiceGroup::Global, VoiceGroup::I, VoiceGroup::II })
    copyFromEditBuffer(transaction, other, vg);
}

void RecallParameterGroups::copyFromEditBuffer(UNDO::Transaction *transaction, const EditBuffer *other, VoiceGroup from)
{
  for(auto &g : other->getParameterGroups(from))
    for(auto &parameter : g->getParameters())
      m_parameters.at(parameter->getID())->copyFrom(transaction, parameter);
}

void RecallParameterGroups::writeDocument(Writer &writer, UpdateDocumentContributor::tUpdateID knownRevision) const
{
  auto changed = getUpdateIDOfLastChange() > knownRevision;
  if(changed)
  {
    writer.writeTag("recall-data", Attribute{ "changed", changed }, [this, &writer, knownRevision] {
      for(auto &parameterpair : getParameters())
        parameterpair.second->writeDocument(writer, knownRevision);
    });
  }
}

UpdateDocumentContributor::tUpdateID RecallParameterGroups::onChange(uint64_t flags)
{
  //m_signalRecallValues.send();
  m_signalRecallValues.deferedSend();
  return UpdateDocumentContributor::onChange(flags);
}

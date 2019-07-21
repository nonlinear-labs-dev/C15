#include <parameters/ModulateableParameter.h>
#include "RecallParameter.h"
#include "RecallParameterGroups.h"
#include "parameters/Parameter.h"
#include "xml/Writer.h"

RecallParameter::RecallParameter(RecallParameterGroups *parent, const int id)
    : UpdateDocumentContributor(parent)
    , m_id{ id }
{
}

void RecallParameter::copyFrom(UNDO::Transaction *transaction, const Parameter *other)
{
  if(other)
  {
    g_assert(m_id == other->getID());
    transaction->addUndoSwap(this, m_recallValue, other->getValue().getRawValue());
    if(auto modP = dynamic_cast<const ModulateableParameter *>(other))
    {
      transaction->addUndoSwap(this, m_recallModAmount, modP->getModulationAmount());
      transaction->addUndoSwap(this, m_recallModSource, modP->getModulationSource());
    }
  }
}

void RecallParameter::writeDocument(Writer &writer, UpdateDocumentContributor::tUpdateID knownRevision) const
{
  if(getUpdateIDOfLastChange() > knownRevision)
  {
    writer.writeTag("param",
                    { Attribute{ "id", to_string(m_id) }, Attribute{ "value", to_string(m_recallValue) },
                      Attribute{ "mod-src", to_string(static_cast<int>(m_recallModSource)) },
                      Attribute{ "mod-amt", to_string(m_recallModAmount) } },
                    []() {});
  }
}

tControlPositionValue RecallParameter::getRecallValue() const
{
  return m_recallValue;
}

tControlPositionValue RecallParameter::getRecallModulationAmount() const
{
  return m_recallModAmount;
}

MacroControls RecallParameter::getRecallModSource() const
{
  return m_recallModSource;
}
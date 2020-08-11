#include <parameters/ModulateableParameter.h>
#include "RecallParameter.h"
#include "RecallParameterGroups.h"
#include "parameters/Parameter.h"
#include "parameters/MacroControlParameter.h"
#include "xml/Writer.h"
#include <xml/Attribute.h>
#include <glib.h>

RecallParameter::RecallParameter(RecallParameterGroups *parent, ParameterId id)
    : UpdateDocumentContributor(parent)
    , m_id { id }
{
}

void RecallParameter::copyFrom(UNDO::Transaction *transaction, const Parameter *other)
{
  if(other)
  {
    g_assert(m_id == other->getID());

    transaction->addUndoSwap(this, m_recallValue, other->getValue().clip(other->getControlPositionValue()));

    if(auto modP = dynamic_cast<const ModulateableParameter *>(other))
    {
      transaction->addUndoSwap(this, m_recallModAmount, modP->getModulationAmount());
      transaction->addUndoSwap(this, m_recallModSource, modP->getModulationSource());
    }

    if(auto mc = dynamic_cast<const MacroControlParameter *>(other))
    {
      transaction->addUndoSwap(this, m_givenName, std::string(mc->getGivenName()));
      transaction->addUndoSwap(this, m_info, std::string(mc->getInfo()));
    }
  }
}

ParameterId RecallParameter::getID() const
{
  return m_id;
}

void RecallParameter::writeDocument(Writer &writer, UpdateDocumentContributor::tUpdateID knownRevision) const
{
  if(getUpdateIDOfLastChange() > knownRevision)
  {
    writer.writeTag("param",
                    { Attribute { "id", m_id.toString() }, Attribute { "value", to_string(m_recallValue) },
                      Attribute { "mod-src", to_string(static_cast<int>(m_recallModSource)) },
                      Attribute { "mod-amt", to_string(m_recallModAmount) } },
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

const std::string &RecallParameter::getGivenName() const
{
  return m_givenName;
}

const std::string &RecallParameter::getInfo() const
{
  return m_info;
}

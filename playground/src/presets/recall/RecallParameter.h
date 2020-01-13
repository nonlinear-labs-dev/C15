#pragma once

#include <libundo/undo/Transaction.h>
#include <http/UpdateDocumentContributor.h>
#include <ParameterId.h>

class RecallParameterGroups;
class Parameter;

class RecallParameter : public UpdateDocumentContributor
{
 public:
  RecallParameter(RecallParameterGroups* parent, ParameterId id);
  void writeDocument(Writer& writer, tUpdateID knownRevision) const override;
  void copyFrom(UNDO::Transaction* transaction, const Parameter* other);

  ParameterId getID() const;

  tControlPositionValue getRecallValue() const;
  MacroControls getRecallModSource() const;
  tControlPositionValue getRecallModulationAmount() const;

 private:
  ParameterId m_id;
  tControlPositionValue m_recallValue = 0;
  tControlPositionValue m_recallModAmount = 0;
  MacroControls m_recallModSource = MacroControls::NONE;

  friend class RecallEditBufferSerializer;
};

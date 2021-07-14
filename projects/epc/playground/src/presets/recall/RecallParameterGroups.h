#pragma once

#include "presets/PresetDualParameterGroups.h"
#include <tools/Signal.h>
#include <ParameterId.h>
#include <unordered_map>

class ParameterGroupSet;
class EditBuffer;
class RecallParameterGroup;
class RecallParameter;

class RecallParameterGroups : public UpdateDocumentContributor
{
 public:
  using tParameterMap = std::unordered_map<ParameterId, std::unique_ptr<RecallParameter>>;

  explicit RecallParameterGroups(EditBuffer *editBuffer);

  RecallParameter *findParameterByID(const ParameterId &id) const;
  RecallParameterGroups::tParameterMap &getParameters();
  const RecallParameterGroups::tParameterMap &getParameters() const;

  void copyFromEditBuffer(UNDO::Transaction *transaction, const EditBuffer *other);
  void copyFromEditBuffer(UNDO::Transaction *transaction, const EditBuffer *other, VoiceGroup vg);
  void writeDocument(Writer &writer, tUpdateID knownRevision) const override;

  tUpdateID onChange(uint64_t flags) override;

 private:
  EditBuffer *m_editBuffer;
  Signal<void> m_signalRecallValues;

  tParameterMap m_parameters;

  friend class EditBuffer;
  friend class RecallEditBufferSerializer;
};

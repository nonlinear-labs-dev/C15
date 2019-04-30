#pragma once

#include "PresetParameterGroups.h"

class ParameterGroupSet;
class EditBuffer;

class RecallParameterGroups : public PresetParameterGroups
{
 public:
  RecallParameterGroups(EditBuffer *editBuffer);

  PresetParameter *findParameterByID(int id);
  const PresetParameter *findParameterByID(int id) const;

  void copyFromEditBuffer(UNDO::Transaction *transaction, const EditBuffer *other);

    tUpdateID onChange(uint64_t flags) override;

    void writeDocument(Writer &writer, tUpdateID knownRevision) const override;

  void copyParamSet(UNDO::Transaction *transaction, const Preset *other);
  void onPresetDeleted(UNDO::Transaction *transaction);

 private:
  Signal<void> m_signalRecallValues;
  Glib::ustring m_origin;

  friend class EditBuffer;
  friend class RecallEditBufferSerializer;
};

#pragma once

#include "AttributesOwner.h"
#include "ParameterId.h"
#include <memory>
#include <unordered_map>

class Preset;
class ParameterGroup;

class PresetParameter;
class PresetParameterGroupSerializer;

class GroupId;

using ParameterPtr = std::unique_ptr<PresetParameter>;

class PresetParameterGroup
{
 public:
  explicit PresetParameterGroup(VoiceGroup vg);
  explicit PresetParameterGroup(const ::ParameterGroup &other);
  PresetParameterGroup(const PresetParameterGroup &other);
  ~PresetParameterGroup();

  // accessors
  [[nodiscard]] PresetParameter *findParameterByID(ParameterId id) const;

  void writeDocument(Writer &writer) const;

  // transactions
  void copyFrom(UNDO::Transaction *transaction, const PresetParameterGroup *other);
  void copyFrom(UNDO::Transaction *transaction, const ::ParameterGroup *other);
  void assignVoiceGroup(UNDO::Transaction *transaction, VoiceGroup vg);

  [[nodiscard]] const std::map<ParameterId, ParameterPtr> &getParameters() const
  {
    return m_parameters;
  }

  [[nodiscard]] VoiceGroup getVoiceGroup() const;

  // algorithm
  void writeDiff(Writer &writer, const GroupId &groupId, const PresetParameterGroup *other) const;

 protected:
  std::map<ParameterId, ParameterPtr> m_parameters;
  VoiceGroup m_voiceGroup;

  friend class PresetParameterGroupSerializer;
};

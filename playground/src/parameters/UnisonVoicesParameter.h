#pragma once
#include "parameters/unison-parameters/UnmodulateableUnisonParameter.h"

class UnisonVoicesParameter : public UnmodulateableUnisonParameter
{
 public:
  explicit UnisonVoicesParameter(ParameterGroup* group, VoiceGroup vg);
  void updateScaling(UNDO::Transaction* transaction, SoundType type);

 protected:
  bool shouldWriteDocProperties(tUpdateID knownRevision) const override;
  bool didScalingChange() const;

  mutable bool m_scalingChanged = false;
};

#pragma once

#include "Button.h"
#include <nltools/Types.h>
#include <libundo/undo/TransactionCreationScope.h>

class Parameter;
class EditBuffer;

class SwitchVoiceGroupButton : public Button
{
 public:
  explicit SwitchVoiceGroupButton(Buttons pos);

  static bool allowToggling(const Parameter* selected, const EditBuffer* editBuffer);

 private:
  void rebuild();

  void onParameterSelectionChanged(Parameter* oldSelected, Parameter* newSelection, SignalOrigin signalType);
  void onVoiceGroupChanged(VoiceGroup newVoiceGroup);
};

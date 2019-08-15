#pragma once
#include "GenericPresetList.h"

class LoadVoiceGroupPresetList : public GenericPresetList
{
public:
  explicit LoadVoiceGroupPresetList(const Point& p);
  void action() override;
};
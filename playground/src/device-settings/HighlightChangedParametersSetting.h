#pragma once
#include "BooleanSetting.h"

class HighlightChangedParametersSetting : public BooleanSetting
{
 public:
  explicit HighlightChangedParametersSetting(Settings& parent);
};

class ForceHighlightChangedParametersSetting : public BooleanSetting
{
 public:
  explicit ForceHighlightChangedParametersSetting(Settings& parent);
  bool persistent() const override;
};
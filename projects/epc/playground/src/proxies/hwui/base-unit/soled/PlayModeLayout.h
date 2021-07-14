#pragma once

#include "PlayOrEditModeLayout.h"

class Label;
class Setting;

class PlayModeLayout : public PlayOrEditModeLayout
{
 private:
  typedef PlayOrEditModeLayout super;

 public:
  PlayModeLayout();
  ~PlayModeLayout() override;
};

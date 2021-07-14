#pragma once

#include "PlayOrEditModeLayout.h"

class Label;
class Setting;

class EditModeLayout : public PlayOrEditModeLayout
{
 private:
  typedef PlayOrEditModeLayout super;

 public:
  EditModeLayout();
  ~EditModeLayout() override;

  EditModeLayout(const EditModeLayout& other) = delete;
  EditModeLayout& operator=(const EditModeLayout&) = delete;
};

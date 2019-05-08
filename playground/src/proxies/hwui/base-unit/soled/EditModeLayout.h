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
  virtual ~EditModeLayout();

 private:
  EditModeLayout(const EditModeLayout& other);
  EditModeLayout& operator=(const EditModeLayout&);
};

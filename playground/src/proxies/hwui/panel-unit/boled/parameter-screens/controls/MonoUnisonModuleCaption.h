#pragma once
#include "ModuleCaption.h"

class MonoUnisonModuleCaption : public ModuleCaption
{
 public:
  using ModuleCaption::ModuleCaption;

 protected:
  void updateText(Parameter *newOne) override;
};

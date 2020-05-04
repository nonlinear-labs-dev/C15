#pragma once

#include <proxies/hwui/buttons.h>
#include "UnmodulatebaleParameterLayouts.h"
#include "ModulateableParameterLayouts.h"

class ModulateableDualVoiceGroupMasterAndSplitPointLayout : public ModulateableParameterSelectLayout2
{
 public:
  using ModulateableParameterSelectLayout2::ModulateableParameterSelectLayout2;

 protected:
  Carousel* createCarousel(const Rect& rect) override;
  ModuleCaption* createModuleCaption() const override;
};

class UnmodulateableDualVoiceGroupMasterAndSplitPointLayout : public UnmodulateableParameterSelectLayout2
{
 public:
  using UnmodulateableParameterSelectLayout2::UnmodulateableParameterSelectLayout2;

 protected:
  Carousel* createCarousel(const Rect& rect) override;
  ModuleCaption* createModuleCaption() const override;
};
#pragma once

#include "ModulateableParameterLayouts.h"
#include "UnmodulatebaleParameterLayouts.h"

class ModulateableUnisonParameterLayout : public ModulateableParameterSelectLayout2
{
 public:
  using ModulateableParameterSelectLayout2::ModulateableParameterSelectLayout2;

 protected:
  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;

  bool isCurrentParameterModulated() const;

 protected:
  Carousel *createCarousel(const Rect &rect) override;

  ModuleCaption *createModuleCaption() const override;
};

class UnmodulateableUnisonParameterLayout : public UnmodulateableParameterSelectLayout2
{
  using UnmodulateableParameterSelectLayout2::UnmodulateableParameterSelectLayout2;

 protected:
  Carousel *createCarousel(const Rect &rect) override;

  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;

  void init() override;

  ModuleCaption *createModuleCaption() const override;
};

#pragma once

#include "UnmodulatebaleParameterLayouts.h"
#include "ModulateableParameterLayouts.h"

class MonoModeParameterLayout : public UnmodulateableParameterSelectLayout2
{
 public:
  MonoModeParameterLayout();

 protected:
  Parameter *getCurrentParameter() const override;

  Carousel *createCarousel(const Rect &rect) override;

  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;

  void init() override;

  ModuleCaption *createModuleCaption() const override;
};

class MonoModeModulateableParameterLayout : public ModulateableParameterSelectLayout2
{
 public:
protected:
  ModuleCaption *createModuleCaption() const override;

public:
  MonoModeModulateableParameterLayout();

 protected:
  Parameter *getCurrentParameter() const override;

  Carousel *createCarousel(const Rect &rect) override;

  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
};

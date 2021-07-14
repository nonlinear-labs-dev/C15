#pragma once

#include "ParameterLayout.h"

class ModulationRouterParameterSelectLayout2 : public virtual ParameterSelectLayout2
{
 public:
  typedef ParameterSelectLayout2 super;
  Carousel *createCarousel(const Rect &rect) override;
  ModulationRouterParameterSelectLayout2();

 protected:
  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
};

class ModulationRouterParameterEditLayout2 : public virtual ParameterEditLayout2
{
 public:
  typedef ParameterEditLayout2 super;
  ModulationRouterParameterEditLayout2();

  ButtonMenu *createMenu(const Rect &rect) override;
};

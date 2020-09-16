#pragma once

#include <proxies/hwui/descriptive-layouts/concrete/menu/menu-items/AnimatedGenericItem.h>

class InitSound : public AnimatedGenericItem
{
 public:
  InitSound(const Rect& rect);
Control * createInfo() override;
};

class InitPart : public AnimatedGenericItem
{
 public:
  InitPart(const Rect& r);
Control * createInfo() override;
};

#pragma once

#include <proxies/hwui/descriptive-layouts/concrete/menu/menu-items/AnimatedGenericItem.h>

class InitSound : public AnimatedGenericItem
{
 public:
  explicit InitSound(const Rect& rect);
};

class InitPart : public AnimatedGenericItem
{
 public:
  explicit InitPart(const Rect& r);
};

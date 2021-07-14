#pragma once

#include <proxies/hwui/descriptive-layouts/concrete/menu/menu-items/EditorItem.h>

class RandomizeItem : public EditorItem
{
 public:
  explicit RandomizeItem(const Rect& rect);
  ~RandomizeItem() override;
  MenuOverlay* createOverlay() override;
};

class RandomizePart : public EditorItem
{
 public:
  explicit RandomizePart(const Rect& r);
  ~RandomizePart() override;

  MenuOverlay* createOverlay() override;
};

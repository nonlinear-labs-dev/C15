#pragma once

#include <proxies/hwui/descriptive-layouts/concrete/menu/menu-items/AnimatedGenericItem.h>
#include <nltools/Types.h>

class ConvertToSoundTypeItem : public AnimatedGenericItem
{
 public:
  explicit ConvertToSoundTypeItem(const Rect& rect, SoundType toType);
};
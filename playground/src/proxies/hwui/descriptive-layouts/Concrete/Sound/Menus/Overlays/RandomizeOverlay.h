#pragma once

#include <proxies/hwui/descriptive-layouts/Concrete/Menu/MenuOverlay/ArrowIncrementDecrementOverlay.h>
#include <device-settings/RandomizeAmount.h>
#include "proxies/hwui/descriptive-layouts/Concrete/Menu/MenuOverlay/ChangeSettingWithCommitOverlay.h"

class RandomizeOverlay : public ChangeSettingWithCommitOverlay<RandomizeAmount>
{
public:
  explicit RandomizeOverlay(const Rect& r);
};
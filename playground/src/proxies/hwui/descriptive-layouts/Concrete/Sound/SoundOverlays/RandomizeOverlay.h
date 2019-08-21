#pragma once

#include <proxies/hwui/descriptive-layouts/Concrete/Sound/SoundOverlays/ArrowIncrementDecrementOverlay.h>
#include <device-settings/RandomizeAmount.h>
#include "ChangeSettingWithCommitOverlay.h"

class RandomizeOverlay : public ChangeSettingWithCommitOverlay<RandomizeAmount>
{
public:
  explicit RandomizeOverlay(const Rect& r);
};
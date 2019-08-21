#pragma once

#include <proxies/hwui/descriptive-layouts/Concrete/Sound/SoundOverlays/ArrowIncrementDecrementOverlay.h>
#include <device-settings/RandomizeAmount.h>

class RandomizeOverlay : public ArrowIncrementDecrementOverlay
{
public:
  explicit RandomizeOverlay(const Rect& r);
  ~RandomizeOverlay() override;

  static RandomizeAmount* getSetting();
  void onCommit(bool down) override;
  void onLeft(bool down) override;
  void onRight(bool down) override;
protected:
  Control* m_label;
};
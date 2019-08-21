#pragma once

#include <proxies/hwui/descriptive-layouts/Concrete/Menu/AbstractMenuItems/EditorItem.h>
#include <proxies/hwui/descriptive-layouts/Concrete/Sound/SoundOverlays/RandomizeOverlay.h>

class RandomizeEditor : public EditorItem
{
public:
  RandomizeEditor();
  ~RandomizeEditor() override;
  ScrollMenuOverlay* createOverlay() override;
  void doAction() override;
};
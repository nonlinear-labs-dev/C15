#pragma once

#include "proxies/hwui/controls/ButtonMenu.h"
#include "device-settings/PresetStoreModeSetting.h"
#include <libundo/undo/Scope.h>
#include <proxies/hwui/controls/ButtonMenuButton.h>

class SoundMenu : public ButtonMenu
{
 public:
  explicit SoundMenu(const Rect &r);

 protected:
  void bruteForce() override;
};

class SingleSoundEditMenuLegacy : public SoundMenu
{
 private:
  typedef SoundMenu super;

 public:
  SingleSoundEditMenuLegacy(const Rect &rect);
  virtual void init();

 protected:
  Font::Justification getDefaultButtonJustification() const override;
};

#pragma once
#include <proxies/hwui/controls/ButtonMenu.h>
#include <nltools/Types.h>
#include <device-settings/LoadModeSetting.h>

class LoadModeMenu : public ControlWithChildren
{
 public:
  explicit LoadModeMenu(const Rect &rect);
  ~LoadModeMenu() override;

  void turn();
  void antiTurn();

 private:
  void bruteForce();

  sigc::connection m_soundTypeConnection;
  sigc::connection m_settingConnection;

  static SoundType getSoundType();
};
